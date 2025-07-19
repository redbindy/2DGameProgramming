#include "Entity.h"

Entity::Entity(
	Sprite* const pSprite,
	Graphics* const pGraphics,
	const int sourceFrameX,
	const int sourceFrameY,
	const int animationFrameCount,
	const int directionalFrameCount
)
	: Image(pSprite, pGraphics, sourceFrameX, sourceFrameY, animationFrameCount, directionalFrameCount)
	, mCollisionType(ECollisionType::CIRCLE)
	, mBoundingRadius(PATCH_SIZE_HALF)
	, mBoundingBox{ XMFLOAT4(-PATCH_SIZE_HALF, -PATCH_SIZE_HALF, PATCH_SIZE_HALF, PATCH_SIZE_HALF) }
	, mVelocity(0.f, 0.f)
	, mDeltaV(0.f, 0.f)
	, mMass(1.f)
	, mHealth(100.f)
	, mbActive(true)
{

}

void Entity::Update(const float frameTime)
{
	XMVECTOR velocity = XMLoadFloat2(&mVelocity);
	XMVECTOR deltaV = XMLoadFloat2(&mDeltaV);
	{
		velocity += deltaV;

		deltaV = XMVectorZero();
	}
	XMStoreFloat2(&mDeltaV, deltaV);
	XMStoreFloat2(&mVelocity, velocity);

	Image::Update(frameTime);
}

bool Entity::TestCollision(const Entity& other, XMFLOAT2& outCollisionVector) const
{
	if (!mbActive || !other.mbActive)
	{
		return false;
	}

	switch (mCollisionType)
	{
	case ECollisionType::CIRCLE:
		switch (other.mCollisionType)
		{
		case ECollisionType::CIRCLE:
			return testCircleCollision(other, outCollisionVector);

		case ECollisionType::BOX:
		case ECollisionType::ROTATED_BOX:
			{
				const bool bResult = other.testSATCollisionBoxCircle(*this, outCollisionVector);

				outCollisionVector.x *= -1.f;
				outCollisionVector.y *= -1.f;

				return bResult;
			}

		default:
			break;
		}
		break;

	case ECollisionType::BOX:
		switch (other.mCollisionType)
		{
		case ECollisionType::CIRCLE:
			return testSATCollisionBoxCircle(other, outCollisionVector);

		case ECollisionType::BOX:
			return testAABBCollision(other, outCollisionVector);

		case ECollisionType::ROTATED_BOX:
			return testOBBCollision(other, outCollisionVector);

		default:
			break;
		}
		break;

	case ECollisionType::ROTATED_BOX:
		switch (other.mCollisionType)
		{
		case ECollisionType::CIRCLE:
			return testSATCollisionBoxCircle(other, outCollisionVector);

		case ECollisionType::BOX:
		case ECollisionType::ROTATED_BOX:
			return testOBBCollision(other, outCollisionVector);

		default:
			break;
		}
		break;

	default:
		return false;
	}

	return false;
}

void Entity::Bounce(const XMFLOAT2& collisionVector, const Entity& other)
{
	const XMVECTOR thisVelocity = XMLoadFloat2(&mVelocity);
	const XMVECTOR otherVelocity = XMLoadFloat2(&other.mVelocity);

	const XMVECTOR vDiff = otherVelocity - thisVelocity;

	const XMVECTOR cV = XMLoadFloat2(&collisionVector);
	const XMVECTOR cUV = XMVector2Normalize(cV);

	const XMVECTOR cUVDotVDiffVector = XMVector2Dot(cUV, vDiff);

	float cUVDotVDiff;
	XMStoreFloat(&cUVDotVDiff, cUVDotVDiffVector);

	float massRatio = 2.f;
	if (mMass != 0.f)
	{
		massRatio *= (other.mMass) / (mMass + other.mMass);
	}

	// 튕김 이전에 호출되고 여전히 충돌 중인 경우 collisionVector를 따라 떨어져 이동
	if (cUVDotVDiff > 0)
	{
		const XMFLOAT2 position = GetPosition();

		const XMVECTOR positionVector = XMLoadFloat2(&position);

		const XMVECTOR nextPositionVector = positionVector - cUV * massRatio;

		XMFLOAT2 nextPosition;
		XMStoreFloat2(&nextPosition, nextPositionVector);

		SetPosition(nextPosition);
	}
	else
	{
		const XMVECTOR thisDeltaV = XMLoadFloat2(&mDeltaV);
		const XMVECTOR deltaV = cUV * massRatio * cUVDotVDiff;

		const XMVECTOR nextDeltaV = thisDeltaV + deltaV;

		XMStoreFloat2(&mDeltaV, nextDeltaV);
	}
}

void Entity::CalculateGravityForce(const Entity& other, const float frameTime)
{
	if (!mbActive || !other.mbActive)
	{
		return;
	}

	const XMFLOAT2 thisCenter = GetCenter();
	const XMFLOAT2 otherCenter = other.GetCenter();

	const XMVECTOR thisCenterVector = XMLoadFloat2(&thisCenter);
	const XMVECTOR otherCenterVector = XMLoadFloat2(&otherCenter);

	const XMVECTOR thisToOther = otherCenterVector - thisCenterVector;

	const XMVECTOR sqauredVector = thisToOther * thisToOther;

	XMFLOAT2 squaredXY;
	XMStoreFloat2(&squaredXY, sqauredVector);

	constexpr float GRAVITY = 200.f;

	const float rr = squaredXY.x + squaredXY.y;

	const float force = GRAVITY * other.GetMass() * mMass / rr;

	const XMVECTOR gravityDirection = XMVector2Normalize(thisToOther);

	const XMVECTOR gravityVector = gravityDirection * force * frameTime;
	const XMVECTOR velocityVector = XMLoadFloat2(&mVelocity);

	const XMVECTOR nextVelocity = velocityVector + gravityVector;

	XMStoreFloat2(&mVelocity, nextVelocity);
}

bool Entity::testCircleCollision(const Entity& other, XMFLOAT2& outCollisionVector) const
{
	const XMVECTOR thisToOther = getVectorRegister(other.GetCenter());

	const XMVECTOR distSquaredVector = XMVector2Dot(thisToOther, thisToOther);

	float centerDistanceSquared;
	XMStoreFloat(&centerDistanceSquared, distSquaredVector);

	const float radiusSum = mBoundingRadius * GetScaleX() + other.mBoundingRadius * other.GetScaleX();
	const float radiusSumSquared = radiusSum * radiusSum;

	if (centerDistanceSquared > radiusSumSquared)
	{
		return false;
	}

	XMStoreFloat2(&outCollisionVector, thisToOther);

	return true;
}

bool Entity::testAABBCollision(const Entity& other, XMFLOAT2& outCollisionVector) const
{
	const BoundingBoxF thisBox = getAABBBox();
	const BoundingBoxF otherBox = other.getAABBBox();

	if (thisBox.box.left > otherBox.box.right
		|| thisBox.box.right < otherBox.box.left
		|| thisBox.box.top > otherBox.box.bottom
		|| thisBox.box.bottom < otherBox.box.top)
	{
		return false;
	}

	const XMVECTOR collisionVectorRegister = getVectorRegister(other.GetCenter());

	XMStoreFloat2(&outCollisionVector, collisionVectorRegister);

	return true;
}

bool Entity::testOBBCollision(const Entity& other, XMFLOAT2& outCollisionVector) const
{
	const bool bOverlappingOnThisAxes = areAllOverlapped(other);
	if (!bOverlappingOnThisAxes)
	{
		return false;
	}

	const bool bOverlappingOnOtherAxes = other.areAllOverlapped(*this);
	if (!bOverlappingOnOtherAxes)
	{
		return false;
	}

	const XMVECTOR collisionVector = getVectorRegister(other.GetCenter());
	XMStoreFloat2(&outCollisionVector, collisionVector);

	return true;
}

bool Entity::testSATCollisionBoxCircle(const Entity& other, XMFLOAT2& outCollisionVector) const
{
	XMVECTOR thisCorners[CORNER_COUNT];
	getCorners(thisCorners);

	const XMFLOAT2 otherCenter = other.GetCenter();

	const XMVECTOR centerVector = XMLoadFloat2(&otherCenter);
	const float scaledRadius = other.mBoundingRadius * other.GetScaleX();

	const XMVECTOR baseVector1 = thisCorners[RIGHT_TOP] - thisCorners[LEFT_TOP];
	const XMVECTOR baseVector1Normalized = XMVector2Normalize(baseVector1);

	float thisMin1;
	float thisMax1;
	float centerProjection1;
	float centerMin1;
	float centerMax1;
	getBoxCircleProjectionResults(
		baseVector1Normalized,
		thisCorners,
		centerVector,
		scaledRadius,
		thisMin1,
		thisMax1,
		centerProjection1,
		centerMin1,
		centerMax1
	);

	if (thisMin1 > centerMax1 || thisMax1 < centerMin1)
	{
		return false;
	}

	const XMVECTOR baseVector2 = thisCorners[LEFT_BOTTOM] - thisCorners[LEFT_TOP];
	const XMVECTOR baseVector2Normalized = XMVector2Normalize(baseVector2);

	float thisMin2;
	float thisMax2;
	float centerProjection2;
	float centerMin2;
	float centerMax2;
	getBoxCircleProjectionResults(
		baseVector2Normalized,
		thisCorners,
		centerVector,
		scaledRadius,
		thisMin2,
		thisMax2,
		centerProjection2,
		centerMin2,
		centerMax2
	);

	if (thisMin2 > centerMax2 || thisMax2 < centerMin2)
	{
		return false;
	}

	XMVECTOR closestCorner;
	bool bVoronoiArea = false;

	if (centerProjection1 < thisMin1 && centerProjection2 < thisMin2)
	{
		closestCorner = thisCorners[LEFT_TOP];
		bVoronoiArea = true;
	}
	else if (centerProjection1 > thisMax1 && centerProjection2 < thisMin2)
	{
		closestCorner = thisCorners[RIGHT_TOP];
		bVoronoiArea = true;
	}
	else if (centerProjection1 > thisMax1 && centerProjection2 > thisMax2)
	{
		closestCorner = thisCorners[RIGHT_BOTTOM];
		bVoronoiArea = true;
	}
	else if (centerProjection1 < thisMin1 && centerProjection2 > thisMax2)
	{
		closestCorner = thisCorners[LEFT_BOTTOM];
		bVoronoiArea = true;
	}

	XMVECTOR collisionVector;
	if (bVoronoiArea)
	{
		const XMVECTOR centerToClosestCorner = closestCorner - centerVector;

		const XMVECTOR centerToClosestDistSquaredVector = XMVector2Dot(centerToClosestCorner, centerToClosestCorner);

		float centerToClosestDistSquared;
		XMStoreFloat(&centerToClosestDistSquared, centerToClosestDistSquaredVector);

		const float scaledRadiusSquared = scaledRadius * scaledRadius;

		if (centerToClosestDistSquared > scaledRadiusSquared)
		{
			return false;
		}

		collisionVector = centerVector - closestCorner;
	}
	else
	{
		collisionVector = getVectorRegister(other.GetCenter());
	}
	XMStoreFloat2(&outCollisionVector, collisionVector);

	return true;
}

bool Entity::areAllOverlapped(const Entity& other) const
{
	XMVECTOR thisCorners[CORNER_COUNT];
	getCorners(thisCorners);

	XMVECTOR otherCorners[CORNER_COUNT];
	other.getCorners(otherCorners);

	const XMVECTOR baseVector1 = thisCorners[ECornerConstant::RIGHT_TOP] - thisCorners[ECornerConstant::LEFT_TOP];
	const XMVECTOR baseVector1Normalized = XMVector2Normalize(baseVector1);

	const bool bResultBase1 = checkProjectionOverlapped(baseVector1Normalized, thisCorners, otherCorners);
	if (!bResultBase1)
	{
		return false;
	}

	const XMVECTOR baseVector2 = thisCorners[ECornerConstant::LEFT_BOTTOM] - thisCorners[ECornerConstant::LEFT_TOP];
	const XMVECTOR baseVector2Normalized = XMVector2Normalize(baseVector2);

	const bool bResultBase2 = checkProjectionOverlapped(baseVector2Normalized, thisCorners, otherCorners);

	return bResultBase2;
}

bool Entity::checkProjectionOverlapped(
	const XMVECTOR baseVectorNormalized,
	const XMVECTOR thisCorners[ECornerConstant::CORNER_COUNT],
	const XMVECTOR otherCorners[ECornerConstant::CORNER_COUNT]
)
{
	XMVECTOR thisProjectionVector = XMVector2Dot(baseVectorNormalized, thisCorners[0]);

	float thisMin;
	XMStoreFloat(&thisMin, thisProjectionVector);

	float thisMax = thisMin;

	XMVECTOR otherProjectionVector = XMVector2Dot(baseVectorNormalized, otherCorners[0]);

	float otherMin;
	XMStoreFloat(&otherMin, otherProjectionVector);

	float otherMax = otherMin;

	for (int i = 1; i < CORNER_COUNT; ++i)
	{
		thisProjectionVector = XMVector2Dot(baseVectorNormalized, thisCorners[i]);

		float thisProjectionResult;
		XMStoreFloat(&thisProjectionResult, thisProjectionVector);

		if (thisProjectionResult < thisMin)
		{
			thisMin = thisProjectionResult;
		}
		else if (thisProjectionResult > thisMax)
		{
			thisMax = thisProjectionResult;
		}

		otherProjectionVector = XMVector2Dot(baseVectorNormalized, otherCorners[i]);

		float otherProjectionResult;
		XMStoreFloat(&otherProjectionResult, otherProjectionVector);

		if (otherProjectionResult < otherMin)
		{
			otherMin = otherProjectionResult;
		}
		else if (otherProjectionResult > otherMax)
		{
			otherMax = otherProjectionResult;
		}
	}

	return thisMin <= otherMax && thisMax >= otherMin;
}

void Entity::getBoxCircleProjectionResults(
	const XMVECTOR baseVectorNormalized,
	const XMVECTOR thisCorners[ECornerConstant::CORNER_COUNT],
	const XMVECTOR center,
	const float scaledRadius,
	float& outBoxMin,
	float& outBoxMax,
	float& outCenterProjection,
	float& outCircleMin,
	float& outCircleMax
)
{
	XMVECTOR thisProjectionVector = XMVector2Dot(baseVectorNormalized, thisCorners[0]);

	float boxMin;
	XMStoreFloat(&boxMin, thisProjectionVector);

	float boxMax = boxMin;

	for (int i = 1; i < CORNER_COUNT; ++i)
	{
		thisProjectionVector = XMVector2Dot(baseVectorNormalized, thisCorners[i]);

		float thisProjectionResult;
		XMStoreFloat(&thisProjectionResult, thisProjectionVector);

		if (thisProjectionResult < boxMin)
		{
			boxMin = thisProjectionResult;
		}
		else if (thisProjectionResult > boxMax)
		{
			boxMax = thisProjectionResult;
		}
	}

	outBoxMin = boxMin;
	outBoxMax = boxMax;

	const XMVECTOR centerProjectionVector = XMVector2Dot(baseVectorNormalized, center);

	float centerProjection;
	XMStoreFloat(&centerProjection, centerProjectionVector);

	outCenterProjection = centerProjection;
	outCircleMin = centerProjection - scaledRadius;
	outCircleMax = centerProjection + scaledRadius;
}
