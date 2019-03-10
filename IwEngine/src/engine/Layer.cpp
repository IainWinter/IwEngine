#include "iw/engine/Layer.h"

namespace IwEngine {
	Layer::Layer(
		const char* name)
		: m_name(name)
	{}

	Layer::~Layer() {}

	int Layer::Initilize() {
		return 0;
	}

	void Layer::Destroy() {}

	void Layer::Update() {}

	void Layer::OnEvent(
		Event& e) 
	{}
}
