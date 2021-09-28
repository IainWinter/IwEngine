#pragma once

struct Throwable
{
	iw::Entity ThrowTarget;
	iw::Entity ThrowRequestor;

	bool Held;
	float Time;
	float Timer;

	Throwable()
		: Held  (false)
		, Time  (1.f)
		, Timer (0.f)
	{}
};
