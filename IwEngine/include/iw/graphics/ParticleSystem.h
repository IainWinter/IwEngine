#pragma once

#include "Mesh.h"
#include "MeshFactory.h"
#include <vector>

namespace iw {
namespace Graphics {
	struct ParticleSystem {
	private:
		Mesh system;

	public:
		ParticleSystem() {
			MeshDescription description;

			description.DescribeBuffer(bName::POSITION, MakeLayout<float>(3));
			//description.DescribeBuffer(bName::UV,       MakeLayout<float>(2));

			VertexBufferLayout instanceLayout(1);
			instanceLayout.Push<float>(4);
			instanceLayout.Push<float>(4);
			instanceLayout.Push<float>(4);
			instanceLayout.Push<float>(4);

			description.DescribeBuffer(bName::UV1, instanceLayout);

			MeshData* meshdata = MakePlane(description, 1, 1);

			system = meshdata->MakeInstance();


		}

		Mesh& GetSystemMesh() {
			return system;
		}
	};
}

	using namespace Graphics;
}
