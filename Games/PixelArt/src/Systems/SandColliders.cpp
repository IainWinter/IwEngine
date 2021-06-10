#include "SandColliders.h"
#include "iw/common/algos/MarchingCubes.h"

void SandColliderSystem::Update()
{
	for (auto batch : m_world->m_batches)
	for (iw::SandChunk* chunk : batch)
	{
		iw::SandChunk::Field& field = chunk->GetField(iw::SandField::SOLID);
		int* cells = field.GetCells<int>();

		auto location = m_world->GetChunkLocation(chunk->m_x, chunk->m_y);
			
		auto& [entities, filledCellCount] = m_cachedColliders[location];

		if (filledCellCount != field.filledCellCount)
		{
			LOG_INFO << field.filledCellCount;

			for (iw::EntityHandle& entity : entities) Space->QueueEntity(entity, iw::func_Destroy);
			entities.clear();

			if (   field.filledCellCount == 0
				|| chunk->m_deleteMe)
			{
				m_cachedColliders.erase(location);
				continue;
			}

			glm::vec3 position = glm::vec3(chunk->m_x, chunk->m_y, 0)
				               / float(m_cellsPerMeter);

			auto polygons = iw::MakePolygonFromField(cells, chunk->m_width, chunk->m_height, 1);

			for (std::vector<glm::vec2>& polygon : polygons)
			{
				std::vector<unsigned> index = iw::TriangulatePolygon(polygon);
				
				if (index.size() == 0) continue;

				for (glm::vec2& v : polygon) {
					v /= float(m_cellsPerMeter);
				}

				iw::Entity entity = Space->CreateEntity<
					iw::Transform, 
					iw::MeshCollider2, 
					iw::CollisionObject, 
					iw::Mesh>();

				iw::Transform*       t = entity.Set<iw::Transform>(position);
				iw::MeshCollider2*   c = entity.Set<iw::MeshCollider2>();
				iw::CollisionObject* o = entity.Set<iw::CollisionObject>();

				o->SetTransform(t);
				o->Collider = c;
				o->IsStatic = true;

				Physics->AddCollisionObject(o);

				c->SetPoints(polygon);
				c->SetTriangles(index);

				m_callback(entity);

				entities.push_back(entity.Handle);
			}

			filledCellCount = field.filledCellCount;
		}
	}
}
