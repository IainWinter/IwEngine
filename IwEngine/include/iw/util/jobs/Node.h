#pragma once

#include "iw/util/iwutil.h"
#include <vector>

namespace iw {
namespace util {
	struct node_in {
		void* value;

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
				memset(value, 0, sizeof(_t));
				*(_t*)value = std::forward<_t>(v);
			}
		}

		template<
			typename _t>
		_t& get() {
			return *(_t*)value;
		}
	};

	struct node;

	struct node_out
		: node_in
	{
		node* next;
		int index;

		IWUTIL_API
		node_out();

		IWUTIL_API
		void link(
			node* node,
			int i);
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

		template<
			typename _t>
		void set(
			unsigned i,
			_t v)
		{
			m_in.at(i).set<_t>(v);
		}

		template<
			typename _t>
		_t& get(
			unsigned i)
		{
			return m_out.at(i).Get<_t>();
		}
	protected:
		template<
			typename _t>
		_t& in(
			unsigned i)
		{
			return m_in.at(i).get<_t>();
		}

		template<
			typename _t>
		void out(
			unsigned i,
			_t v)
		{
			node_out& o = m_out.at(i);

			if (o.next) {
				o.next->set<_t>(o.index, v);
			}

			o.set<_t>(v);
		}
	};
}

	using namespace util;
}
