#pragma once

#include "IwGraphics.h"
#include "iw/renderer/Pipeline.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/ref.h"
#include <vector>

namespace iw {
namespace Graphics {
	enum class ShaderType {
		INVALID,
		VERTEX,
		GEOMETRY,
		FRAGMENT,
		COMPUTE
	};

	struct Shader {
	protected:
		struct ShaderSource {
			ShaderType Type;
			size_t SourceSize;
			char* Source;
		};

		std::vector<ShaderSource> m_source;
		IPipeline* m_handle;

	public:
		IWGRAPHICS_API
		Shader();

		IWGRAPHICS_API
		void AddShader(
			ShaderType type,
			const char* source);

		IWGRAPHICS_API
		IPipeline* Handle() const;

		IWGRAPHICS_API
		bool IsInitialized() const;

		IWGRAPHICS_API virtual void Initialize(const iw::ref<IDevice>& device);
		IWGRAPHICS_API         void Use       (const iw::ref<IDevice>& device) const;
	};
}

	using namespace Graphics;
}
