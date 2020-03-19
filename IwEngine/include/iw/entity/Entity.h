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
			const EntityHandle& other)
		{
			return Handle == other;
		}

		bool operator!=(
			const EntityHandle& other)
		{
			return !operator==(other);
		}

		size_t Index() {
			return Handle.Index;
		}

		short Version() {
			return Handle.Version;
		}

		bool Alive() {
			return Handle.Alive;
		}

		template<
			typename _c,
			typename... _args>
		_c* AddComponent(
			_args&&... args)
		{
#ifdef IW_DEBUG
		if (!Space) {
			LOG_ERROR << "Entity has no space!";
			return nullptr;
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
			return;
		}
#endif
			Space->RemoveComponent<_c>(Handle);
		}

		template<
			typename _c,
			typename... _args>
		_c* SetComponent(
			_args&&... args)
		{
#ifdef IW_DEBUG
		if (!Space) {
			LOG_ERROR << "Entity has no space!";
			return nullptr;
		}
#endif
			return Space->SetComponent<_c>(Handle, std::forward<_args>(args)...);
		}

		template<
			typename _c>
		_c* FindComponent() {
#ifdef IW_DEBUG
		if (!Space) {
			LOG_ERROR << "Entity has no space!";
			return nullptr;
		}
#endif
			return Space->FindComponent<_c>(Handle);
		}

		template<
			typename _c>
		bool HasComponent() {
#ifdef IW_DEBUG
		if (!Space) {
			LOG_ERROR << "Entity has no space!";
			return false;
		}
#endif
			return Space->HasComponent<_c>(Handle);
		}
	};
}

	using namespace ECS;
}
