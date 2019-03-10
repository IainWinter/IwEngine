#include "iw/engine/Layer.h"

namespace IwEngine {
	class ImGuiLayer
		: public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		int Initilize();
		void Destroy();
		void Update();

		void OnEvent(
			Event& e);
	};
}
