#pragma once

namespace IwGraphics {
	class IndexBuffer {
	private:
		unsigned int m_renderId;
		const void* m_data;
		unsigned int m_count;

	public:
		IndexBuffer(
			const void* data,
			unsigned int count);

		~IndexBuffer();

		void Bind()   const;
		void Unbind() const;
		void Draw()   const;

		inline unsigned int GetCount() const {
			return m_count;
		}
	};
}
