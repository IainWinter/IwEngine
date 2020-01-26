#include "iw/util/jobs/pipeline.h"

namespace iw {
	pipeline::pipeline()
		: m_root(nullptr)
		, m_current(nullptr)
	{}

	pipeline::~pipeline() {
		for (node* n : m_index) {
			delete n;
		}
	}

	void pipeline::execute() {
		for (node* node : m_index) {
			node->execute();
		}
	}
}
