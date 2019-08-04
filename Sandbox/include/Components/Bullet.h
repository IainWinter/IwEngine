#pragma once

enum BulletType {
	LINE
};

struct Bullet {
	BulletType Type;
	float      Speed;
};
