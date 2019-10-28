#pragma once

namespace IW {
inline namespace Asset {
	class IAssetLoader {
	public:
		virtual ~IAssetLoader() {}

		virtual size_t GetType() = 0;

		virtual void* Load(
			const char* filepath) = 0;

		virtual void Give(
			const char* filename,
			void* asset) = 0;
	};
}
}
