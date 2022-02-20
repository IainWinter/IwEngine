#pragma once

struct Throwable
{
	/*iw::Entity ThrowTarget;
	iw::Entity ThrowRequestor;*/

	entity ThrowTarget;
	entity ThrowRequestor;

	bool Held = false;
	float Time = 1.f;
	float Timer = 0.f;

	Throwable()
		: Held  (false)
		, Time  (1.f)
		, Timer (0.f)
	{}
};
