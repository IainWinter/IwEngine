#pragma once

namespace iwutil {
	using id_t = unsigned int;

	class identifiable {
	private:
		static id_t m_next_id;
		static id_t m_id;
	public:
		static id_t id() {
			return m_id;
		}
	};

	id_t identifiable::m_next_id = 0;
	id_t identifiable::m_id = ++identifiable::m_next_id;
}

//
//namespace iwutil {
//	using typeid_t = unsigned int;
//
//	template<typename T>
//	class type_identifier {
//	private:
//		constexpr static typeid_t m_id = 0;
//
//		template<int N>
//		struct wrter {
//			constexpr int add(N) {
//				return N;
//			}
//
//			static constexpr int value = N;
//		};
//
//
//	};
//}
//
//
//
//namespace iwutil {
//	using id_t = unsigned int;
//
//	/**
//	* Static type to store a type id. Can be used at compile/runtime to replace typeid(T).hash_code().
//	* @tparam T Type to id.
//	*/
//	template<typename T>
//	class type_identifier3 {
//	private:
//		constexpr static id_t m_id = 0;
//
//		template<int N>
//		struct flag {
//			friend constexpr int add1(flag<N>);
//		};
//
//		template<int N>
//		struct writer {
//			friend constexpr int add1(flag<N>) {
//				return N;
//			}
//
//			static constexpr int value = N;
//		};
//
//		template<int N, class = char[noexcept(add1(flag<N>())) ? +1 : -1]>
//		static int constexpr reader(int, flag<N>) {
//			return N;
//		}
//
//		template<int N>
//		static int constexpr reader(float, flag<N>, int R = reader(0, flag<N - 1>())) {
//			return R;
//		}
//
//		static int constexpr reader(float, flag<0>) {
//			return 0;
//		}
//
//		template<int N = 1, int C = reader(0, flag<0xfffffff>())>
//		static int constexpr next(int R = writer<C + N>::value) {
//			return R;
//		}
//	public:
//		/**
//		* Returns the id of T.
//		* @return The id of T.
//		*/
//		static constexpr id_t get_id() {
//			return m_id;
//		}
//
//		/**
//		* Returns K's sub id in T.
//		* @tparam K Type to id.
//		* @return The sub id of K in T.
//		*
//		*/
//		template<typename K>
//		static constexpr id_t get_id() {
//			return next() - 1;
//		}
//	};
//
//	/**
//	* Provides functions for getting type ids.
//	* Useful in templated classes where distinguishing between different types is needed.
//	* @tparam T Generally the class being inherited to.
//	*/
//	template<typename T>
//	class identify_by {
//	public:
//		/**
//		* Returns the id of T.
//		* @return T's id
//		*/
//		constexpr static id_t get_id() {
//			return type_identifier<T>::get_id();
//		}
//
//		/**
//		* Returns K's sub id in T.
//		* @tparam K The type to id.
//		* @return The sub id of K in T.
//		*/
//		template<typename K>
//		constexpr static id_t get_type_id() {
//			return type_identifier<T>::template get_id<K>();
//		}
//	};
//}