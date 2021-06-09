#include "SandColliders.h"
#include "iw/common/algos/MarchingCubes.h"

void SandColliderSystem::Update()
{
	for (auto batch : m_world->m_batches)
	for (iw::SandChunk* chunk : batch)
	{
		iw::SandChunk::Field& field = chunk->GetField(iw::SandField::SOLID);
		int* cells = field.GetCells<int>();

		auto& [entity, filledCellCount] = m_cachedColliders[m_world->GetChunkLocation(chunk->m_x, chunk->m_y)];

		if (filledCellCount != field.filledCellCount)
		{
			filledCellCount = field.filledCellCount;

			for (int j = chunk->m_height - 1; j >= 0; j--)
			{
				std::stringstream sb;
				for (int i = 0; i < chunk->m_width;  i++)
				{
					sb << cells[i + j * chunk->m_width];
				}

				LOG_INFO << sb.str();
			}

			auto                   polygon = iw::MakePolygonFromField(cells, chunk->m_width, chunk->m_height, 1);
			std::vector<unsigned>  index   = iw::TriangulatePolygon(polygon[0]);

			if (index.size() == 0) continue;

			for (glm::vec2& v : polygon[0])
			{
				v /= 10.f;
			}

			iw::MeshCollider2* collider;

			if (entity) {
				collider = entity.Find<iw::MeshCollider2>();
			}

			else {
				entity = Space->CreateEntity<
					iw::Transform, 
					iw::MeshCollider2, 
					iw::CollisionObject, 
					iw::Mesh>();

				iw::Transform*       t = entity.Set<iw::Transform>(glm::vec3(chunk->m_x, chunk->m_y, 0) / float(m_cellsPerMeter));
				iw::MeshCollider2*   c = entity.Set<iw::MeshCollider2>();
				iw::CollisionObject* o = entity.Set<iw::CollisionObject>();

				o->SetTransform(t);
				o->Collider = c;
				o->IsStatic = true;

				Physics->AddCollisionObject(o);

				collider = c;
			}

			collider->SetPoints(polygon[0]);
			collider->SetTriangles(index);

			m_callback(entity);
		}
	}
}
