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
		typename _t,
		typename... _args>
	pipeline& first(
		_args... args)
	{
		delete m_root;
		m_root = new _t(std::forward<_args>(args)...);
		m_current = m_root;

		m_index.clear();
		m_index.push_back(m_current);

		return *this;
	}

	template<
		typename _t,
		typename... _args>
	pipeline& then(
		unsigned a,
		unsigned b,
		_args... args)
	{
		if (m_current) {
			node* n = new _t(std::forward<_args>(args)...);
			m_current->link(n, a, b);
			m_current = n;

			m_index.push_back(m_current);
		}

		return *this;
	}

	IWUTIL_API
	pipeline& init(/*multithread options*/);

	template<
		typename _t>
	pipeline& set(
		unsigned nodeIndex,
		unsigned index,
		_t value)
	{
		m_index.at(nodeIndex)->set<_t>(index, value);
		return *this;
	}

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
