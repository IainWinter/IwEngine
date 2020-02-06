#pragma once

#include "IwEntity.h"
#include "PrefabComponent.h"

namespace IW {
namespace ECS {
	struct Prefab {
	private:
		size_t ComponentCount;
		PrefabComponent Component[];
	};

}
	using namespace ECS;
}
