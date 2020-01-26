#include "iw/util/jobs/node.h"

namespace iw {
	node_in::node_in()
		: value(nullptr)
	{}

	node_in::~node_in() {
		free(value);
	}

	void node_out::link(
		node* node,
		int index)
	{
		links.emplace_back(node, index);
	}

	void node_out::unlink(
		node* node,
		int index)
	{
		auto itr = links.begin();
		for (; itr != links.end(); itr++) {
			if (itr->next == node && itr->index == index) {
				break;
			}
		}

		links.erase(itr);
	}

	node_link::node_link(
		node* next,
		int index)
		: next(next)
		, index(index)
	{}

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

	void node::unlink(
		node* node,
		unsigned a,
		unsigned b)
	{
		m_out.at(a).unlink(node, b);
	}
}
