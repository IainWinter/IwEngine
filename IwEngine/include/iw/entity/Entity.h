#pragma once

#include "Space.h"

namespace IW {
namespace ECS {
	struct Entity {
		EntityHandle Handle;
		Space& Space;

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
			if (Space.FindComponent<_c>(Handle)) {
				return Space.SetComponent<_c>(Handle, std::forward<_args>(args)...);
			}

			return Space.AddComponent<_c>(Handle, std::forward<_args>(args)...);
		}

		template<
			typename _c>
		void RemoveComponent() {
			Space.RemoveComponent<_c>(Handle);
		}

		template<
			typename _c,
			typename... _args>
		_c* SetComponent(
			_args&&... args)
		{
			return Space.SetComponent<_c>(Handle, std::forward<_args>(args)...);
		}

		template<
			typename _c>
		_c* FindComponent() {
			return Space.FindComponent<_c>(Handle);
		}

		template<
			typename _c>
		bool HasComponent() {
			return Space.HasComponent<_c>(Handle);
		}
	};
}

	using namespace ECS;
}
