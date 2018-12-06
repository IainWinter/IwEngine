#pragma once

#include <iterator>
#include <tuple>

namespace iwutil {
	template<typename... T>
	class tuple {
	private:
		using tuple_t = std::tuple<T...>;

		tuple_t m_data;

		template<typename F, int... Index>
		void foreach(F functor, std::index_sequence<Index...>) {
			auto l = { (functor(std::get<Index>(m_data)), 0)... };
		}
	public:
		dynamic_tuple() = default;

		dynamic_tuple(T&&... args)
			: m_data(std::forward<T>(args)...) {}

		dynamic_tuple& operator=(dynamic_tuple& copy) {
			m_data = std::tuple<T...>(copy.m_data);
			return *this;
		}

		bool operator==(const dynamic_tuple& tup) const {
			return m_data == tup.m_data;
		}

		bool operator!=(const dynamic_tuple& tup) const {
			return m_data != tup.m_data;
		}

		template<typename K>
		K& get() {
			constexpr std::size_t index = this->get_type_id<K>();
			return std::get<index>(m_data);
		}

		template<typename K>
		void set(K&& value) {
			get<K>() = value;
		}

		template<typename S>
		S as_struct() {
			return make_struct<S, tuple_t>(m_data);
		}

		void forall(void(*func)(T&...)) {
			func(get<T>()...);
		}

		template<typename F>
		void foreach(F&& functor) {
			foreach(std::move(functor), std::make_index_sequence<sizeof...(T)>());
		}
	};

	namespace functors {
		struct increment {
			template<typename T>
			void operator () (T&& t) {
				t++;
			}
		};

		struct decrement {
			template<typename T>
			void operator () (T&& t) {
				t--;
			}
		};
	}
}