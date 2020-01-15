#include "iw/util/jobs/node.h"

namespace iw {
	node_in::node_in()
		: value(nullptr)
	{}

	node_in::~node_in() {
		free(value);
	}

	node_out::node_out()
		: node_in()
		, next(nullptr)
		, index(-1)
	{}

	void node_out::link(
		node* node,
		int i)
	{
		next  = node;
		index = i;
	}

	node::node(
		unsigned in,
		unsigned out)
	{
		m_in.resize(in);
		m_out.resize(out);
	}

	void node::link(
		node* node,
		unsigned a,
		unsigned b)
	{
		m_out.at(a).link(node, b);
	}
}
