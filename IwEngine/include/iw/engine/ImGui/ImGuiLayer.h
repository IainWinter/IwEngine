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

		bool On(WindowResizedEvent&       event);
		bool On(MouseMovedEvent&          event);
		bool On(MouseButtonPressedEvent&  event);
		bool On(MouseButtonReleasedEvent& event);
	private:
		//bool OnMouseButtonDown();
		//bool OnMouseButtonUp();
		//bool OnKeyButtonDown();
		//bool OnKeyButtonUp();
		//bool OnWindowResized();
	};
}
