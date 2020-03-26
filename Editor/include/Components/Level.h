#pragma once

#include "Enemy.h"
#include "iw/math/vector2.h"
//#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "Player.h"
#include "LevelDoor.h"
#include <vector>

struct REFLECT Level {
	REFLECT std::string StageName = "";

	REFLECT bool CameraFollow = false;

	REFLECT std::vector<Enemy>       Enemies;
	REFLECT std::vector<iw::vector2> Positions;

	REFLECT iw::vector2 InPosition;
	REFLECT iw::vector2 OutPosition;
	REFLECT iw::vector2 LevelPosition;

	REFLECT LevelDoor Door = {};

	//REFLECT std::vector<iw::SphereCollider> Spheres;
	REFLECT std::vector<iw::PlaneCollider>  Planes;

	//iw::Model Stage;
};
