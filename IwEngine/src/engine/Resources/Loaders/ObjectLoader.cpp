#include "iw/engine/Resources/Loaders/ObjectLoader.h"
#include "iw/log/logger.h"
#include "tinyobjloader/tiny_obj_loader.h"
#include <string>

namespace IwEngine {
	Object* ObjLoader::Load(
		const char* path)
	{
		Object* obj = new Object(path);

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn;
		std::string err;

		tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path);

		if (!warn.empty()) {
			LOG_WARNING << warn;
		}

		if (!err.empty()) {
			LOG_ERROR << err;
		}

		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

					if (idx.vertex_index != -1) {
						auto itr = std::find(obj->Vertices.begin(),
							obj->Faces.begin(), idx.vertex_index);

						if (itr == obj->Faces.end()) {
							obj->Vertices.push_back(iwm::vector3(
								attrib.vertices[3 * idx.vertex_index + 0],
								attrib.vertices[3 * idx.vertex_index + 1],
								attrib.vertices[3 * idx.vertex_index + 2]
							));

							obj->Colors.push_back(iwm::vector3(
								attrib.colors[3 * idx.vertex_index + 0],
								attrib.colors[3 * idx.vertex_index + 1],
								attrib.colors[3 * idx.vertex_index + 2]
							));
						}

						obj->Faces.push_back(idx.vertex_index);
					}

					if (idx.normal_index != -1) {
						obj->Normals.push_back(iwm::vector3(
							attrib.normals[3 * idx.normal_index + 0],
							attrib.normals[3 * idx.normal_index + 1],
							attrib.normals[3 * idx.normal_index + 2]
						));
					}

					if (idx.texcoord_index != -1) {
						obj->TexCoords.push_back(iwm::vector2(
							attrib.texcoords[2 * idx.texcoord_index + 0],
							attrib.texcoords[2 * idx.texcoord_index + 1]
						));
					}
				}



				index_offset += fv;

				// per-face material
				//shapes[s].mesh.material_ids[f];
			}
		}

		return obj;
	}

	void ObjLoader::Release(
		Object* resource)
	{
		delete resource;
	}
}
