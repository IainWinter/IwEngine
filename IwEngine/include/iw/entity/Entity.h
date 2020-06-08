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

		operator bool() {
			return *this != EntityHandle::Empty;
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
		bool Has() {
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
