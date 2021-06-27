#include "SandColliders.h"

void SandColliderSystem::Update()
{
	for (auto batch : m_world->m_batches)
	for (iw::SandChunk* chunk : batch)
	{
		iw::SandChunk::Field& field = chunk->GetField(iw::SandField::COLLISION);
		bool* cells = field.GetCells<bool>();

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

			auto polygons = iw::MakePolygonFromField(cells, chunk->m_width, chunk->m_height);

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

				if (MadeColliderCallback) {
					MadeColliderCallback(entity);
				}

				entities.push_back(entity.Handle);
			}

			filledCellCount = field.filledCellCount;
		}
	}
}

void SandColliderSystem::CutWorld(
	const glm::vec2& p1, 
	const glm::vec2& p2) 
{
	// Get chunk locations that contain line segment
	// Get colliders from those chunks
	// Cut those colliders and create tiles from those

	// sort line by x

	glm::vec2 cp1 = p1 * (float)m_cellsPerMeter;
	glm::vec2 cp2 = p2 * (float)m_cellsPerMeter;

	auto [min, max] = iw::AABB2(cp1, cp2);
	auto [cxmin, cymin] = m_world->GetChunkLocation(min.x, min.y);
	auto [cxmax, cymax] = m_world->GetChunkLocation(max.x, max.y);

	std::vector<std::pair<int, int>> locations;

	for (int x = cxmin; x <= cxmax; x++)
	for (int y = cymin; y <= cymax; y++)
	{
		// b   c
		// 
		// a   d

		std::pair<int, int> location = { x, y };

		iw::SandChunk* chunk = m_world->GetChunkDirect(location);

		if (!chunk || m_cachedColliders.find(location) == m_cachedColliders.end()) continue;

		glm::vec2 a = glm::vec2(chunk->m_x,                  chunk->m_y);
		glm::vec2 b = glm::vec2(chunk->m_x,                  chunk->m_y + chunk->m_height);
		glm::vec2 c = glm::vec2(chunk->m_x + chunk->m_width, chunk->m_y + chunk->m_height);
		glm::vec2 d = glm::vec2(chunk->m_x + chunk->m_width, chunk->m_y);

		if (   iw::SegmentIntersection(cp1, cp2, a, b).second
			|| iw::SegmentIntersection(cp1, cp2, b, c).second
			|| iw::SegmentIntersection(cp1, cp2, c, d).second
			|| iw::SegmentIntersection(cp1, cp2, d, a).second)
		{
			locations.push_back(location);
		}
	}

	for (std::pair<int, int>& location : locations) 
	{
		for (iw::EntityHandle handle : m_cachedColliders.find(location)->second.first) 
		{
			glm::vec2 chunkPos = glm::vec2(
				location.first  * m_world->m_chunkWidth,
				location.second * m_world->m_chunkHeight
			);

			//glm::vec2 l1 = (cp1 - chunkPos) / float(m_cellsPerMeter);
			//glm::vec2 l2 = (cp2 - chunkPos) / float(m_cellsPerMeter);

			iw::MeshCollider2* mesh =  Space->FindComponent<iw::MeshCollider2>(handle);
			iw::Transform      tran = *Space->FindComponent<iw::Transform>(handle);

			std::vector<glm::vec2> polygon = mesh->m_points;
			iw::TransformPolygon(polygon, &tran);

			iw::polygon_cut cut = iw::CutPolygon(polygon, mesh->m_index, p1, p2);

			if (CutColliderCallback) {
				CutColliderCallback(mesh, cut);
			}
		}
	}
}
