#pragma once

#include <unordered_set>

namespace iwutil {
	template<typename _k, typename _v>
	class bimap {
	private:
		using fpair_t = std::pair<_k, _v*>;
		using bpair_t = std::pair<_v, _k*>;
		using setf_t = std::unordered_set<fpair_t>;
		using setb_t = std::unordered_set<bpair_t>;

		setf_t m_forward_set;
		setf_t m_backward_set;
	public:
		_v& at(const _k& key) {
			return *(m_forward_set.find(key).first);
		}

		_k& at(const _v& value) {
			return *m_backward_set.find(value).first;
		}

		void emplace(const _k& key, const _v& value) {
			auto f_itr = m_forward_set.emplace({ key, nullptr }).first;
			_k* k_ptr = &(f_itr->first);

			auto b_itr = m_backward_set.emplace({value, k_ptr}).first;
			_v* v_ptr = &(b_itr->first);

			f_itr->second = v_ptr;
		}

		void erase(const _k& key) {
			_v* v_ptr = m_forward_set.find(key).second;
			m_backward_set.erase(&v_ptr);
			m_forward_set.erase(key);
		}

		void erase(const _v& value) {
			_k* v_ptr = m_backward_set.find(value).second;
			m_backward_set.erase(value);
			m_forward_set.erase(&v_ptr);
		}

		typename setf_t::iterator find(const _k& key) {
			return m_forward_set.find(key);
		}

		typename setb_t::iterator find(const _v& value) {
			return m_backward_set.find(value);
		}

		typename setf_t::iterator begin() {
			return m_forward_set.begin();
		}

		typename setf_t::iterator end() {
			return m_forward_set.end();
		}

		typename setb_t::iterator begin_value() {
			return m_backward_set.begin();
		}

		typename setb_t::iterator end_value() {
			return m_backward_set.end();
		}
	};
}