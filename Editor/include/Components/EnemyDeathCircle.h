#pragma once

enum class CircleEffects {
	ENEMY_DEATH_CIRCLE,
	BULLET_REVERSAL
};

struct EffectCircle {
	CircleEffects Effect = CircleEffects::ENEMY_DEATH_CIRCLE;
	float Radius = 4.0f;
	float FadeTime = 0.4f;
	float Timer = 0.0f;
};
