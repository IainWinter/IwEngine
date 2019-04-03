#pragma once

#include "iwevents.h"
#include <functional>
#include <vector>
#include <memory>
#include <algorithm>

namespace iwevents {
	/**
	* @brief Signals can be used to call many
	* functions at once with the same arguments.
	*
	* Signals can link many functions together. When arguments gets published 
	* to the signal, all subscribed functions are notified.
	* Only functions that have the signals arguments can be added!
	*
	* @tparam _args_t The arguments of the functions that can
	* be subscribed to the signal.
	*/
	template<
		typename... _args_t>
	class signal {
	private:
		using invoke_type = bool(*)(std::weak_ptr<void>&, _args_t...);
		using func_type = std::pair<std::weak_ptr<void>, invoke_type>;

		std::vector<func_type> m_functions;

		template<
			void(*_function)(_args_t...)>
		static bool invoke(
			std::weak_ptr<void>&,
			_args_t... args)
		{
			_function(args...);
			return true; //No instance so can not expire.
		}

		template<
			typename _t,
			void(_t::*_function)(_args_t...)>
		static bool invoke(
			std::weak_ptr<void>& instance,
			_args_t... args)
		{
			bool not_expired = !instance.expired();
			if (not_expired) {
				std::shared_ptr<_t> instance_ptr
					= std::static_pointer_cast<_t>(instance.lock());
				(instance_ptr.get()->*_function)(args...);
			}

			return not_expired;
		}

		void remove_function(
			std::function<bool(const func_type&)> find_function_to_remove)
		{
			auto function_to_remove = std::remove_if(
				m_functions.begin(),
				m_functions.end(),
				find_function_to_remove);
			m_functions.erase(function_to_remove, m_functions.end());
		}
	public:
		signal() {
			m_functions = std::vector<func_type>();
		}

		/**
		* @brief Publishes an event to the
		* signal with the specified arguments.
		*
		* Notifies all subscribed functions to the signal
		* with the specified arguments.
		*
		* @param args The arguments to be passed into the
		* subscribed functions.
		*/
		void publish(
			_args_t... args)
		{
			size_t f_count = m_functions.size();
			std::vector<func_type> non_expired_functions
				= std::vector<func_type>();
			non_expired_functions.reserve(f_count);

			for (size_t i = 0; i < f_count; i++) {
				auto func = m_functions[i];
				bool not_expired = func.second(func.first, args...);
				if (not_expired) {
					non_expired_functions.push_back(m_functions[i]);
				}
			}

			m_functions.swap(non_expired_functions);
		}

		/**
		* @brief Subscribes a free function to the signal.
		*
		* Subscribes a function to the signal. It will be called 
		* whenever arguments get published to the signal.
		*
		* @tparam _function A function pointer to be subscribed to the signal.
		*/
		template<
			void(*_function)(_args_t...)>
		void subscribe() {
			unsubscribe<_function>();
			m_functions.emplace_back(std::weak_ptr<void>(), &invoke<_function>);
		}

		/**
		* @brief Subscribes a public member function to the signal.
		*
		* Subscribes a public member function to the signal. If the 
		* instance is deleted, the signal with forget about 
		* the function automaticly.
		*
		* @tparam _t The type of instance.
		* @tparam _function A member function pointer to be subscribed to the signal.
		*/
		template<
			typename _t,
			void(_t::*_function)(_args_t...)>
		void subscribe(
			std::shared_ptr<_t> instance)
		{
			unsubscribe<_t, _function>(instance);
			m_functions.emplace_back(std::move(instance), &invoke<_t, _function>);
		}

		/**
		* @brief Unsubscribes a free function from the signal.
		*
		* Unsubscribes a free function from the signal.
		*
		* @tparam _function A pointer to a free function to the subscribed to the signal.
		*/
		template<
			void(*_function)(_args_t...)>
		void unsubscribe() {
			remove_function([](const func_type& func_type) {
				return func_type.second == &invoke<_function> && !func_type.first.lock();
			});
		}

		/**
		* @brief Unsubscribes a public member function from the signal.
		*
		* Unsubscribes a public member function from the signal.
		*
		* @tparam _t The type of instance.
		* @param instance The instance of _t to be subscribed to the signal.
		*/
		template<
			typename _t,
			void(_t::*_function)(_args_t...)>
		void unsubscribe(
			std::shared_ptr<_t> instance)
		{
			remove_function([instance{ std::move(instance) }](const func_type& func_type) {
				return func_type.second == &invoke<_t, _function> && func_type.first.lock() == instance;
			});
		}

		/**
		* @brief Unsubscribes all public member function from the instance from the signal.
		*
		* Unsubscribes all public member function from the instance from the signal.
		*
		* @tparam _t The type of instance.
		* @param instance The instance of _t to be subscribed to the signal.
		*/
		template<
			typename _t>
		void unsubscribe(
			std::shared_ptr<_t> instance)
		{
			remove_function([instance{ std::move(instance) }](const func_type& func_type) {
				return func_type.first.lock() == instance;
			});
		}
	};
}
