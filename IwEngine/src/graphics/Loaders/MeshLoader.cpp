#include "iw/graphics/Loaders/MeshLoader.h"
//#include "iw/reflection/serialization/JsonSerializer.h"
#include "iw/log/logger.h"

namespace iw {
namespace Graphics {
	Mesh* MeshLoader::LoadAsset(
		std::string path)
	{
		LOG_ERROR << "no impl";
		//MeshConfig loaded;
		//JsonSerializer(path).Read<MeshConfig>(loaded);



		return new Mesh();
	}
}
}
