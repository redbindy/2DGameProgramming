#pragma once

#include "Entity.h"

struct HeroDTO
{
	float x;
	float y;
	float deltaX;
	float deltaY;
};

class Hero final : public Entity
{
public:
	Hero(
		Sprite* const pSprite,
		Game* const pGame,
		const int sourceFrameX,
		const int sourceFrameY,
		const int animationFrameCount,
		const int directionalFrameCount
	);
	virtual ~Hero() = default;

	virtual void Update(const float frameTime) override;
	virtual void UpdateAI(const float frameTime, const Entity& other) const override;
	virtual void OnAttacked(const int damage) override;

	virtual void DrawPatch() const;

	inline void OnShield();

	HeroDTO GetDTO() const;
	void SetDTO(HeroDTO heroDTO);

private:
	Image mShieldImage;
	bool mbShield;
	float mShieldTimer;
};

inline void Hero::OnShield()
{
	mbShield = true;
}

inline HeroDTO Hero::GetDTO() const
{
	HeroDTO ret;
	ret.x = GetX();
	ret.y = GetY();

	const XMFLOAT2 deltaV = GetDeltaV();
	ret.deltaX = deltaV.x;
	ret.deltaY = deltaV.y;

	return ret;
}

inline void Hero::SetDTO(HeroDTO heroDTO)
{
	SetX(heroDTO.x);
	SetY(heroDTO.y);
	SetDeltaV(heroDTO.deltaX, heroDTO.deltaY);
}
