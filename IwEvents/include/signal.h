#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include "iwevents.h"

namespace iwevents {
	/**
	* @breif Signals can be used to call many functions at once with the same arguments.
	*
	* Signals can link many functions together. When arguments gets published 
	* to the signal, all subscribed functions are notified. Only functions that
	* have the signals arguments can be added!
	*
	* @param Args The arguments of the functions that can be subscribed to the signal.
	*/
	template<typename... Args>
	class signal {
	private:
		using invoke_type = bool(*)(std::weak_ptr<void>&, Args...);
		using func_type = std::pair<std::weak_ptr<void>, invoke_type>;

		std::vector<func_type> m_functions;

		template<void(*Function)(Args...)>
		static bool invoke(std::weak_ptr<void>&, Args... args) {
			Function(args...);
			return true; //No instance so can not expire.
		}

		template<typename T, void(T::*Function)(Args...)>
		static bool invoke(std::weak_ptr<void>& instance, Args... args) {
			bool not_expired = !instance.expired();
			if (not_expired) {
				std::shared_ptr<T> instance_ptr = std::static_pointer_cast<T>(instance.lock());
				(instance_ptr.get()->*Function)(args...);
			}

			return not_expired;
		}

		void remove_function(std::function<bool(const func_type&)> find_function_to_remove) {
			auto function_to_remove = std::remove_if(m_functions.begin(), m_functions.end(), find_function_to_remove);
			m_functions.erase(function_to_remove, m_functions.end());
		}
	public:
		signal() {
			m_functions = std::vector<func_type>();
		}

		/**
		* @breif Publishes an event to the signal with the specified arguments.
		*
		* Notifies all subscribed functions to the signal with the specified arguments.
		*
		* @param args The arguments to be passed into the subscribed functions.
		*/
		void publish(Args... args) {
			size_t f_count = m_functions.size();
			std::vector<func_type> non_expired_functions = std::vector<func_type>();
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
		* @breif Subscribes a free function to the signal.
		*
		* Subscribes a function to the signal. It will be called 
		* whenever arguments get published to the signal.
		*
		* @param Function A function pointer to be subscribed to the signal.
		*/
		template<void(*Function)(Args...)>
		void subscribe() {
			unsubscribe<Function>();
			m_functions.emplace_back(std::weak_ptr<void>(), &invoke<Function>);
		}

		/**
		* @breif Subscribes a public member function to the signal.
		*
		* Subscribes a public member function to the signal. If the 
		* instance is deleted, the signal with forget about 
		* the function automaticly.
		*
		* @param ClassType The type of instance.
		* @param Function A member function pointer to be subscribed to the signal.
		*/
		template<typename ClassType, void(ClassType::*Function)(Args...)>
		void subscribe(std::shared_ptr<ClassType> instance) {
			unsubscribe<ClassType, Function>(instance);
			m_functions.emplace_back(std::move(instance), &invoke<ClassType, Function>);
		}

		/**
		* @breif Unsubscribes a free function from the signal.
		*
		* Unsubscribes a free function from the signal.
		*
		* @param Function A pointer to a free function to the subscribed to the signal.
		*/
		template<void(*Function)(Args...)>
		void unsubscribe() {
			remove_function([](const func_type& func_type) {
				return func_type.second == &invoke<Function> && !func_type.first.lock();
			});
		}

		/**
		* @breif Unsubscribes a public member function from the signal.
		*
		* Unsubscribes a public member function from the signal.
		*
		* @param ClassType The type of instance.
		* @param instance The instance of ClassType to be subscribed to the signal.
		*/
		template<typename ClassType, void(ClassType::*Function)(Args...)>
		void unsubscribe(std::shared_ptr<ClassType> instance) {
			remove_function([instance{ std::move(instance) }](const func_type& func_type) {
				return func_type.second == &invoke<ClassType, Function> && func_type.first.lock() == instance;
			});
		}

		/**
		* @breif Unsubscribes all public member function from the instance from the signal.
		*
		* Unsubscribes all public member function from the instance from the signal.
		*
		* @param ClassType The type of instance.
		* @param instance The instance of ClassType to be subscribed to the signal.
		*/
		template<typename ClassType>
		void unsubscribe(std::shared_ptr<ClassType> instance) {
			remove_function([instance{ std::move(instance) }](const func_type& func_type) {
				return func_type.first.lock() == instance;
			});
		}
	};
}