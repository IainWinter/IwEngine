#pragma once
#include "node.h"

namespace iw {
namespace util {
class pipeline {
private:
	node* m_root;
	node* m_current;
	std::vector<node*> m_index;

public:
	IWUTIL_API
	pipeline();

	IWUTIL_API
	~pipeline();

	template<
		typename _t>
	pipeline& first(
		_t* node)
	{
		delete m_root;
		m_root = node;
		m_current = m_root;

		m_index.clear();
		m_index.push_back(m_current);

		return *this;
	}

	template<
		typename _t>
	pipeline& then(
		_t* node,
		unsigned a = 0,
		unsigned b = 0)
	{
		if (m_current) {
			m_current->link(node, a, b);
			m_current = node;

			m_index.push_back(m_current);
		}

		return *this;
	}

	IWUTIL_API
	pipeline& init(/*multithread options*/);

	IWUTIL_API
	void execute();

	template<
		typename _t>
	_t& result(
		unsigned index)
	{
		return m_index.back()->get<_t>(index);
	}
};
}

	using namespace util;
}
