#include "iw/renderer/Renderer.h"

namespace IwRenderer {
	void Renderer::Submit(
		const DrawItem& drawitem) 
	{
		drawitem.State.ShaderProgram->Use();
		drawitem.State.Mesh->Draw(*drawitem.State.Transformation);
	}
}
