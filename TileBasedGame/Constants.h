#pragma once

constexpr float FRAME_RATE = 144.f;
constexpr float MIN_FRAME_RATE = 10.f;
constexpr float MIN_FRAME_TIME = 1.f / FRAME_RATE;
constexpr float MAX_FRAME_TIME = 1.f / MIN_FRAME_RATE;

template<class COM>
inline void SafeRelease(COM*& p)
{
	if (p != nullptr)
	{
		p->Release();
		p = nullptr;
	}
}