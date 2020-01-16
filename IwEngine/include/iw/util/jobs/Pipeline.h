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
		return first<_t>(new _t(std::forward<_args>(args)...));
	}

	template<
		typename _t,
		typename... _args>
	pipeline& then(
		unsigned a,
		unsigned b,
		_args... args)
	{
		return then<_t>(new _t(std::forward<_args>(args)...));
	}

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
		unsigned a,
		unsigned b,
		_t* node)
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
