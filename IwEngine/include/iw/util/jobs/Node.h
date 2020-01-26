#pragma once

#include "iw/util/iwutil.h"
#include <vector>

namespace iw {
namespace util {
	struct node_in {
	private:
		void* value;

	public:
		IWUTIL_API
		node_in();

		IWUTIL_API
		virtual ~node_in();

		template<
			typename _t>
		void set(
			_t v)
		{
			void* ptr = realloc(value, sizeof(_t));
			if (!ptr) {
				value = malloc(sizeof(_t));
			}

			else {
				value = ptr;
			}

			if (value) {
				new (value) _t(v);
			}
		}

		template<
			typename _t>
		_t& get() {
			return *(_t*)value;
		}
	};

	struct node;
	struct node_link;

	struct node_out
		: node_in
	{
		std::vector<node_link> links;

		IWUTIL_API
		void link(
			node* node,
			int index);

		IWUTIL_API
		void unlink(
			node* node,
			int index);
	};

	struct node_link {
		node* next;
		int index;

		IWUTIL_API
		node_link(
			node* next = nullptr,
			int index = -1);
	};

	struct node {
	private:
		std::vector<node_in>  m_in;
		std::vector<node_out> m_out;

	public:
		IWUTIL_API
		node(
			unsigned in,
			unsigned out);

		IWUTIL_API
		virtual void execute() = 0;

		IWUTIL_API
		void link(
			node* node,
			unsigned a,
			unsigned b);

		IWUTIL_API
		void unlink(
			node* node,
			unsigned a,
			unsigned b);

		template<
			typename _t>
		void set(
			unsigned index,
			_t value)
		{
			m_in.at(index).set<_t>(value);
		}

		template<
			typename _t>
		_t& get(
			unsigned index)
		{
			return m_out.at(index).get<_t>();
		}
	protected:
		template<
			typename _t>
		_t& in(
			unsigned index)
		{
			return m_in.at(index).get<_t>();
		}

		template<
			typename _t>
		void out(
			unsigned index,
			_t value)
		{
			node_out& node = m_out.at(index);
			if (node.links.size() != 0) {
				for (node_link& link : node.links) {
					link.next->set<_t>(link.index, value);
				}
			}

			node.set<_t>(value);
		}
	};
}

	using namespace util;
}
