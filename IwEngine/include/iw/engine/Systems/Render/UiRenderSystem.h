#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/graphics/Mesh.h"
#include "iw/graphics/Camera.h"
#include "iw/engine/Components/UiElement_temp.h"

namespace iw {
namespace Engine {
	class UiRenderSystem
		: public System<Transform, Mesh, UiElement>
	{
	public:
		struct Components {
			Transform* Transform;
			Mesh* Mesh;
			UiElement* Ui;
		};

		Camera* m_camera;

	public:
		IWENGINE_API
		UiRenderSystem(
			Camera* m_camera);

		IWENGINE_API
		void Update(
			EntityComponentArray& view);
	};
}

	using namespace Engine;
}
