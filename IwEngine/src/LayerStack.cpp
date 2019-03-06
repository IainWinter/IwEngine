#include "LayerStack.h"

namespace IwEngine {
	LayerStack::LayerStack() {
		m_insert = m_layers.begin();
	}

	LayerStack::~LayerStack() {
		for (Layer* l : m_layers) {
			delete l;
		}
	}

	void LayerStack::PushLayer(
		Layer* layer)
	{
		m_insert = m_layers.emplace(m_insert, layer);
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
			layer->Stop();
			m_layers.erase(itr);
			m_insert--;
		}
	}

	void LayerStack::PopOverlay(
		Layer* overlay)
	{
		iterator itr = std::find(begin(), end(), overlay);
		if (itr != end()) {
			overlay->Stop();
			m_layers.erase(itr);
		}
	}
}
