#pragma once

#include "Mesh.h"
#include "MeshFactory.h"
#include "iw/util/memory/linear_allocator.h"
#include <functional>
#include <vector>

namespace iw {
namespace Graphics {
	struct StaticParticle {

	};

	struct TimedParticle {
		float LifeTime;
	};

	template<
		typename _p>
	struct Particle {
		Transform Transform;
		_p Instance;

		Particle(
			const iw::Transform& transform,
			const _p& particle)
			: Transform(transform)
			, Instance(particle)
		{}
	};

	template<
		typename _p = StaticParticle>
	struct ParticleSystem;

	template<
		typename _p = StaticParticle>
	struct ParticleSystem {
	public:
		using particle_t = Particle<_p>;
		using func_update = std::function<bool(ParticleSystem<_p>*, particle_t*, unsigned)>;

	private:
		std::vector<particle_t> m_particles; // but the template works for now
		std::vector<particle_t> m_spawn;
		std::vector<unsigned>   m_delete;

		Mesh m_mesh;
		bool m_needsToUpdateBuffer;

		func_update m_update;
		_p m_prefab;

		Transform* m_transform;
		Transform  m_lastTransform;
		linear_allocator m_alloc;

	public:
		ParticleSystem() {
			m_needsToUpdateBuffer = false;

			m_update = [](ParticleSystem<_p>* s, particle_t*, unsigned) {
				return false;
			};

			m_transform = nullptr;
		}

		~ParticleSystem() = default;

		      Mesh& GetParticleMesh()       { return m_mesh; }
		const Mesh& GetParticleMesh() const { return m_mesh;}

		      Transform* GetTransform()       { return m_transform; }
		const Transform* GetTransform() const { return m_transform; }

		void SetTransform(
			Transform* transform)
		{
			m_transform = transform;
		}

		void SetParticleMesh(
			const Mesh& mesh)
		{
			MeshDescription description;

			VertexBufferLayout instanceM(1);
			instanceM.Push<float>(4);
			instanceM.Push<float>(4);
			instanceM.Push<float>(4);
			instanceM.Push<float>(4);

			description.DescribeBuffer(bName::POSITION, MakeLayout<float>(3));
			description.DescribeBuffer(bName::NORMAL,   MakeLayout<float>(3));
			description.DescribeBuffer(bName::UV,       MakeLayout<float>(2));
			description.DescribeBuffer(bName::UV1, instanceM);

			mesh.Data()->ConformMeshData(description);

			m_mesh = mesh;
		}

		void SetPrefab(
			const _p& prefab)
		{
			m_prefab = prefab;
		}

		void SetUpdate(
			func_update update)
		{
			m_update = update;
		}

		void UpdateParticleMesh() {
			if (   m_needsToUpdateBuffer
				|| (m_transform && m_lastTransform != *m_transform))
			{
				m_needsToUpdateBuffer = false;
				m_lastTransform.Position = m_transform ? m_transform->Position : 0;
				m_lastTransform.Scale    = m_transform ? m_transform->Scale    : 0;
				m_lastTransform.Rotation = m_transform ? m_transform->Rotation : quaternion::identity;
				m_lastTransform.SetParent(m_transform->Parent());

				for (unsigned i : m_delete) {
					m_particles.erase(m_particles.begin() + i);
				}

				for (const particle_t& p : m_spawn) {
					m_particles.push_back(p);
				}

				// Set parents after to not screw up child list in parent from stack dealloc

				if (m_transform) {
					for (unsigned i = 1; i <= m_spawn.size(); i++) {
						m_particles[m_particles.size() - i].Transform.SetParent(m_transform);
					}
				}

				unsigned count = m_particles.size();

				if (    m_delete.size() != 0
					|| m_spawn .size() != 0)
				{
					m_alloc.resize(count * sizeof(matrix4));
				}

				m_delete.clear();
				m_spawn .clear();

				m_alloc.reset();

				if (count == 0) {
					return;
				}

				matrix4* models = m_alloc.alloc<matrix4>(count);

				for (unsigned i = 0; i < count; i++) {
					models[i] = m_particles[i].Transform.WorldTransformation(); // somewhere there is 'World' where it should be local :<
				}

				m_mesh.Data()->SetBufferDataPtr(bName::UV1, count, models);
			}
		}

		void SpawnParticle(
			const Transform& transform = Transform())
		{
			SpawnParticle(m_prefab, transform);
		}

		void SpawnParticle(
			const _p& particle,
			const Transform& transform = Transform())
		{
			m_spawn.emplace_back(transform, particle);
			m_needsToUpdateBuffer |= true;
		}

		void DeleteParticle(
			unsigned index)
		{
			m_delete.push_back(index);
			m_needsToUpdateBuffer |= true;
		}

		void Update() {
			if (!m_update) {
				//LOG_WARNING << "Tried to update particle system with no update function!";
				return;
			}

			m_needsToUpdateBuffer |= m_update(this, m_particles.data(), (unsigned)m_particles.size());
		}

		const std::vector<particle_t>& Particles() const {
			return m_particles;
		}

		std::vector<particle_t>& Particles() {
			return m_particles;
		}

		unsigned ParticleCount() const {
			return m_particles.size() + m_spawn.size() - m_delete.size();
		}

		bool HasParticleMesh() const {
			return m_mesh.Data() != nullptr;
		}
	};

	using StaticPS = ParticleSystem<StaticParticle>;
}

	using namespace Graphics;
}
