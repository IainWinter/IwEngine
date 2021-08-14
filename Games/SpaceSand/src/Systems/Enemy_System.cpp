#include "Systems/Enemy_System.h"

//int EnemySystem::Initialize()
//{
//	// create enemy ship prefabs
//
//	// create 
//
//	return 0;
//}

void EnemySystem::FixedUpdate()
{
	Space->Query<iw::Rigidbody, EnemyShip>().Each(
		[&](
			iw::EntityHandle entity,
			iw::Rigidbody* body,
			EnemyShip* enemy)
		{
			//enemy moves twoarsd target

			// enemy shoots after a random amount of time at the player




		}
	);
}
