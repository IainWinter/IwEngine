#include "iw/graphics/ComputeShader.h"
#include "iw/util/enum/val.h"

namespace iw {
namespace Graphics {
	void ComputeShader::Initialize(
		const iw::ref<IDevice>& device)
	{
		if (IsInitialized()) {
			LOG_WARNING << "Tried to initialize a compute shader twice!";
			return;
		}

		if (m_source.size() != 1) {
			LOG_WARNING << "Tried to initialize a compute shader with " << m_source.size() << " shaders. Only 1 is allowed!";
			return;
		}

		ShaderSource& shader = m_source.front();

		if (shader.Type != ShaderType::COMPUTE) {
			LOG_WARNING << "Tried to initialize a compute shader with an invalid shader type " << iw::val(shader.Type);
			return;
		}

		IComputeShader* compute = device->CreateComputeShader(shader.Source);

		m_handle = device->CreateComputePipeline(compute);
	}
}
}
