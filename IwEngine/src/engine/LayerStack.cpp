#include "iw/engine/LayerStack.h"

namespace IwEngine {
	LayerStack::LayerStack()
		: m_insertIndex(0)
	{}

	LayerStack::~LayerStack() {
		for (Layer* l : m_layers) {
			delete l;
		}
	}

	void LayerStack::PushLayer(
		Layer* layer)
	{
		m_layers.emplace(begin() + m_insertIndex, layer);
		m_insertIndex++;
	}

	void LayerStack::PushOverlay(
		Layer* overlay)
	{
		m_layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(
		Layer* layer)
	{
		iterator itr = std::find(begin(), end(), layer);
		if (itr != end()) {
			m_layers.erase(itr);
			m_insertIndex--;
		}
	}

	void LayerStack::PopOverlay(
		Layer* overlay)
	{
		iterator itr = std::find(begin(), end(), overlay);
		if (itr != end()) {
			m_layers.erase(itr);
		}
	}
}
