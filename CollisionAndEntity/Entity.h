#pragma once

#include <DirectXMath.h>

#include "Image.h"

using namespace DirectX;

enum class ECollisionType
{
	NONE,
	CIRCLE,
	BOX,
	ROTATED_BOX
};

enum EBoundingBoxConstant
{
	LEFT,
	TOP,
	RIGHT,
	BOTTOM
};

enum ECornerConstant
{
	CORNER_COUNT = 4,
	LEFT_TOP = 0,
	RIGHT_TOP = 1,
	RIGHT_BOTTOM = 2,
	LEFT_BOTTOM = 3
};

union BoundingBoxF
{
	XMFLOAT4 xm4;

	struct
	{
		XMFLOAT2 leftTop;
		XMFLOAT2 rightBottom;
	} xm2;

	struct
	{
		float left;
		float top;
		float right;
		float bottom;
	} box;
};

class Entity : public Image
{
public:
	Entity(
		Sprite* const pSprite,
		Graphics* const pGraphics,
		const int sourceFrameX,
		const int sourceFrameY,
		const int animationFrameCount,
		const int directionalFrameCount
	);
	~Entity() = default;

	inline bool IsActive() const;
	inline void Activate();

	virtual void Update(const float frameTime) override;
	virtual void UpdateAI(const float frameTime, const Entity& other) const = 0;
	virtual void OnAttacked(const int damage) = 0;

	bool TestCollision(const Entity& other, XMFLOAT2& outCollisionVector) const;
	inline bool IsOutsideRect(const RECT rect) const;

	void Bounce(const XMFLOAT2& collisionVector, const Entity& other);
	void CalculateGravityForce(const Entity& other, const float frameTime);

	inline ECollisionType GetCollisionType() const;

	inline XMFLOAT2 GetCenter() const;

	inline float GetBoundingRadius() const;

	inline XMFLOAT2 GetVelocity() const;
	inline void SetVelocity(const float x, const float y);

	inline XMFLOAT2 GetDeltaV() const;
	inline void AddDeltaV(const float x, const float y);

	inline float GetMass() const;
	inline void SetMass(const float mass);

	inline float GetHealth() const;

private:
	ECollisionType mCollisionType;

	float mBoundingRadius;
	BoundingBoxF mBoundingBox;

	XMFLOAT2 mVelocity;
	XMFLOAT2 mDeltaV;
	float mMass;

	float mHealth;

	bool mbActive;

private:
	inline XMVECTOR __vectorcall getVectorRegister(const XMFLOAT2 to) const;
	inline BoundingBoxF __vectorcall getTransformedBox(const XMFLOAT2 translation, const XMFLOAT2 scale, const float rotation) const;
	inline BoundingBoxF __vectorcall getTransformedBox() const;
	inline BoundingBoxF __vectorcall getAABBBox() const;

	bool testCircleCollision(const Entity& other, XMFLOAT2& outCollisionVector) const;
	bool testAABBCollision(const Entity& other, XMFLOAT2& outCollisionVector) const;
	bool testOBBCollision(const Entity& other, XMFLOAT2& outCollisionVector) const;
	bool testSATCollisionBoxCircle(const Entity& other, XMFLOAT2& outCollisionVector) const;

	inline void getCorners(XMVECTOR outCorners[ECornerConstant::CORNER_COUNT]) const;
	bool areAllOverlapped(const Entity& other) const;

	static bool checkProjectionOverlapped(
		const XMVECTOR baseVectorNormalized,
		const XMVECTOR thisCorners[ECornerConstant::CORNER_COUNT],
		const XMVECTOR otherCorners[ECornerConstant::CORNER_COUNT]
	);

	static void getBoxCircleProjectionResults(
		const XMVECTOR baseVectorNormalized,
		const XMVECTOR thisCorners[ECornerConstant::CORNER_COUNT],
		const XMVECTOR center,
		const float scaledRadius,
		float& outBoxMin,
		float& outBoxMax,
		float& outCenterProjection,
		float& outCircleMin,
		float& outCircleMax
	);
};

inline bool Entity::IsActive() const
{
	return mbActive;
}

inline void Entity::Activate()
{
	mbActive = true;
}

inline ECollisionType Entity::GetCollisionType() const
{
	return mCollisionType;
}

inline XMFLOAT2 Entity::GetCenter() const
{
	const XMFLOAT2 worldPosition = GetPosition();

	const XMFLOAT2 center(worldPosition.x + PATCH_SIZE_HALF, worldPosition.y + PATCH_SIZE_HALF);

	return center;
}

inline float Entity::GetBoundingRadius() const
{
	return mBoundingRadius;
}

inline XMFLOAT2 Entity::GetVelocity() const
{
	return mVelocity;
}

inline void Entity::SetVelocity(const float x, const float y)
{
	mVelocity = XMFLOAT2(x, y);
}

inline XMFLOAT2 Entity::GetDeltaV() const
{
	return mDeltaV;
}

inline void Entity::AddDeltaV(const float x, const float y)
{
	mDeltaV.x += x;
	mDeltaV.y += y;
}

inline float Entity::GetMass() const
{
	return mMass;
}

inline void Entity::SetMass(const float mass)
{
	mMass = mass;
}

inline float Entity::GetHealth() const
{
	return mHealth;
}

inline XMVECTOR __vectorcall Entity::getVectorRegister(const XMFLOAT2 to) const
{
	const XMFLOAT2 center = GetCenter();

	const XMVECTOR fromRegister = XMLoadFloat2(&center);
	const XMVECTOR toRegister = XMLoadFloat2(&to);

	return toRegister - fromRegister;
}

inline BoundingBoxF __vectorcall Entity::getTransformedBox(const XMFLOAT2 translation, const XMFLOAT2 scale, const float rotation) const
{
	const XMVECTOR leftTopVector = XMLoadFloat2(&mBoundingBox.xm2.leftTop);
	const XMVECTOR rightBottomVector = XMLoadFloat2(&mBoundingBox.xm2.rightBottom);

	constexpr XMFLOAT2 transformingOrigin(PATCH_SIZE_HALF, PATCH_SIZE_HALF);
	const XMVECTOR transformingOriginVector = XMLoadFloat2(&transformingOrigin);

	const XMFLOAT2 center = GetCenter();
	const XMVECTOR centerVector = XMLoadFloat2(&center);

	const XMVECTOR scaleVector = XMLoadFloat2(&scale);

	const XMMATRIX transformMatrix = XMMatrixTransformation2D(transformingOriginVector, 0.f, scaleVector, transformingOriginVector, rotation, centerVector);

	const XMVECTOR transformedLeftTop = XMVector2Transform(leftTopVector, transformMatrix);
	const XMVECTOR transformedRightBottom = XMVector2Transform(rightBottomVector, transformMatrix);

	BoundingBoxF result;
	XMStoreFloat2(&result.xm2.leftTop, transformedLeftTop);
	XMStoreFloat2(&result.xm2.rightBottom, transformedRightBottom);

	return result;
}

inline BoundingBoxF __vectorcall Entity::getTransformedBox() const
{
	return getTransformedBox(GetCenter(), GetScale(), GetRotationAngle());
}

inline BoundingBoxF __vectorcall Entity::getAABBBox() const
{
	return getTransformedBox(GetCenter(), GetScale(), 0);
}

inline void Entity::getCorners(XMVECTOR outCorners[ECornerConstant::CORNER_COUNT]) const
{
	const BoundingBoxF box = getTransformedBox();
	const XMVECTOR boxVector = XMLoadFloat4(&box.xm4);

	outCorners[ECornerConstant::LEFT_TOP] = XMVectorSwizzle<EBoundingBoxConstant::LEFT, EBoundingBoxConstant::TOP, 0, 0>(boxVector);
	outCorners[ECornerConstant::RIGHT_TOP] = XMVectorSwizzle<EBoundingBoxConstant::RIGHT, EBoundingBoxConstant::TOP, 0, 0>(boxVector);
	outCorners[ECornerConstant::RIGHT_BOTTOM] = XMVectorSwizzle<EBoundingBoxConstant::RIGHT, EBoundingBoxConstant::BOTTOM, 0, 0>(boxVector);
	outCorners[ECornerConstant::LEFT_BOTTOM] = XMVectorSwizzle<EBoundingBoxConstant::LEFT, EBoundingBoxConstant::BOTTOM, 0, 0>(boxVector);
}

inline bool Entity::IsOutsideRect(const RECT rect) const
{
	const XMFLOAT2 position = GetPosition();

	const float right = position.x + PATCH_SIZE * GetScaleX();
	const float bottom = position.y + PATCH_SIZE * GetScaleY();

	return position.x > rect.right
		|| right < rect.left
		|| position.y > rect.bottom
		|| bottom < rect.top;
}