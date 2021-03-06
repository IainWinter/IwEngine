#pragma once

#include "Space.h"

namespace iw {
namespace ECS {
	struct Entity {
		EntityHandle Handle;
		Space* Space;

		Entity()
			: Handle(EntityHandle::Empty)
			, Space(nullptr)
		{}

		Entity(
			EntityHandle handle,
			ECS::Space* space)
			: Handle(handle)
			, Space(space)
		{}

		bool operator==(
			const EntityHandle& other) const
		{
			return Handle == other;
		}

		bool operator!=(
			const EntityHandle& other) const
		{
			return !operator==(other);
		}

		operator bool() const {
			return *this != EntityHandle::Empty;
		}

		size_t Index() const {
			return Handle.Index;
		}

		short Version() const {
			return Handle.Version;
		}

		bool Alive() const {
			return Handle.Alive;
		}

		void Destroy() {
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return;
			}
#endif
			Space->DestroyEntity(Index());
		}

		void Kill() {
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return;
			}
#endif
			Space->KillEntity(Handle);
		}

		void Revive() {
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return;
			}
#endif
			Space->ReviveEntity(Handle);
		}

		template<
			typename _c,
			typename... _args>
		_c* Add(
			_args&&... args)
		{
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return false;
			}
#endif
			if (Space->FindComponent<_c>(Handle)) {
				return Space->SetComponent<_c>(Handle, std::forward<_args>(args)...);
			}

			return Space->AddComponent<_c>(Handle, std::forward<_args>(args)...);
		}

		template<
			typename _c>
		void RemoveComponent() {
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return false;
			}
#endif
			Space->RemoveComponent<_c>(Handle);
		}

		template<
			typename _c,
			typename... _args>
		_c* Set(
			_args&&... args)
		{
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return false;
			}
#endif
			return Space->SetComponent<_c>(Handle, std::forward<_args>(args)...);
		}

		template<
			typename _c>
		_c* Find() {
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return false;
			}
#endif
			return Space->FindComponent<_c>(Handle);
		}

		template<
			typename _c>
		bool Has() const {
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return false;
			}
#endif
			return Space->HasComponent<_c>(Handle);
		}
	};

	const func_EntityChange func_Destroy = [](Entity e) {
		e.Destroy();
	};
}

	using namespace ECS;
}
