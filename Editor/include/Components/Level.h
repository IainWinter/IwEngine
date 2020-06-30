#pragma once

#include "Enemy.h"
#include "iw/math/vector2.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/CapsuleCollider.h"
#include "Player.h"
#include "LevelDoor.h"
#include "ModelPrefab.h"
#include <vector>

struct REFLECT Level {
	REFLECT bool CameraFollow = false;
	REFLECT iw::vector3 InPosition;
	REFLECT iw::vector3 LevelPosition;

	REFLECT std::vector<Enemy>       Enemies;
	REFLECT std::vector<iw::vector2> Positions;

	REFLECT std::vector<LevelDoor> Doors;
	REFLECT std::vector<iw::vector2> DoorPositions;

	REFLECT std::vector<iw::SphereCollider>  Spheres;
	REFLECT std::vector<iw::CapsuleCollider> Capsules;
	REFLECT std::vector<iw::PlaneCollider>   Planes;

	REFLECT std::vector<ModelPrefab> Models;

	//iw::Model Stage;
};
