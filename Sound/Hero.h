#pragma once

#include "Entity.h"

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
	~Hero() = default;

	virtual void Update(const float frameTime) override;
	virtual void UpdateAI(const float frameTime, const Entity& other) const override;
	virtual void OnAttacked(const int damage) override;

	virtual void Draw() const;

	inline void OnShield();

private:
	Image mShieldImage;
	bool mbShield;
	float mShieldTimer;
};

inline void Hero::OnShield()
{
	mbShield = true;
}