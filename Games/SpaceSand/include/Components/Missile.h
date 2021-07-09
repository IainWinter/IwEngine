#pragma once

struct Missile {
	float tX = 0;
	float tY = 0;
	float TurnRad = 0.025f;

	float Timer = 0;
	float WaitTime = 0.5f;
	float BurnTime = 3;

	int TileId = 0;
};
