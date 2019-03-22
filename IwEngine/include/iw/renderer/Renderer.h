#pragma once

#include "Mesh.h"
#include "ShaderProgram.h"
#include "iw/math/matrix4.h"

namespace IwRenderer {
	//States
	//	Material
	//	ShaderProgram
	//	VAO
	//	IBO
	//	Textures
	//	DrawStyle
	//	Transformation

	struct StateGroup {
		const Mesh* Mesh;
		const ShaderProgram* ShaderProgram;
		const iwm::matrix4* Transformation;
	};

	enum DrawStyle {
		POINTS,
		LINES,
		TRIANGLES,
		TRIANGLE_FAN
	};

	//Draw Commands
	//	Vert count ie
	//	Draw type
	struct DrawCommand {
		DrawStyle Style;
	};

	//States
	struct DrawItem {
		StateGroup State;
		DrawCommand Command;
	};

	class Renderer {
	private:
		//device

	public:
		void Submit(const DrawItem& drawitem);
	};
}
