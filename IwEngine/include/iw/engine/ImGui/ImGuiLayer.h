#include "iw/engine/Layer.h"

namespace IwEngine {
	class IWENGINE_API ImGuiLayer
		: public Layer
	{
	private:
		int counter = 0;
	public:
		ImGuiLayer();
		~ImGuiLayer();

		int Initilize();
		void Destroy();
		void Update();

		void OnEvent(
			Event& e);
	private:
		//bool OnMouseButtonDown();
		//bool OnMouseButtonUp();
		//bool OnKeyButtonDown();
		//bool OnKeyButtonUp();
		//bool OnWindowResized();
	};
}
