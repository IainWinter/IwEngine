#pragma once

namespace iw {
namespace Engine {
	struct CameraController {
		bool Active = true;
		bool MakeOrthoOnInit = false;

		virtual Camera* GetCamera() = 0;
	};
}

	using namespace Engine;
}
