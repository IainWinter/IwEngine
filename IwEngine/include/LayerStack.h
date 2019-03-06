#pragma once

#include "Core.h"
#include "Layer.h"
#include <vector>

namespace IwEngine {
	class IWENGINE_API LayerStack {
	public:
		using iterator = std::vector<Layer*>::iterator;
	private:
		struct {
			std::vector<Layer*> m_layers;
			iterator m_insert;
		};

	public:
		LayerStack();
		~LayerStack();

		void PushLayer(
			Layer* layer);

		void PopLayer(
			Layer* layer);

		void PushOverlay(
			Layer* overlay);

		void PopOverlay(
			Layer* overlay);

		iterator begin() {
			return m_layers.begin();
		}

		iterator end() {
			return m_layers.end();
		}
	};
}
