#pragma once

namespace IwGraphics {
	class VertexBuffer {
	private:
		unsigned int m_renderId;
		const void* m_data;

	public:
		VertexBuffer(
			const void* data,
			unsigned int size);

		~VertexBuffer();

		void Bind() const;
		void Unbind() const;
	};
}
