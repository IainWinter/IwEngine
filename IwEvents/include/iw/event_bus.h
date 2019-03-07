#pragma once

#include "signal.h"

namespace iwevents {
	/*
	Fixes doxygen problem where it does not detect that event_bus<_event_t, _others_t...> 
	inherits from event_bus<_event_t> and event_bus<_others_t>....
	*/
	template<typename... _events_t>
	class event_bus;

	/**
	* @breif Event bus specilization for many event types.
	*
	* The event bus allows member functions to be added and removed from many
	* signals at once. Classes must publicly expose functions of the
	* signature 'void _t::process(const Event&)' to be added. Free functions can
	* also be added, but do not need any specific name.
	*
	* When events are published, the event bus notifies all of the event
	* handlers connected to the bus with the specified event type and arguments.
	*
	* @tparam _event_t The list of events managed by the event bus.
	*/
	template<typename _event_t, typename... _others_t>
	class event_bus<_event_t, _others_t...> : event_bus<_event_t>, event_bus<_others_t>... {
	public:
		/**
		* @breif Subscribes a class instance to the event bus.
		*
		* Subscribes all public member functions of the signature 
		* 'void _t::process(const Event&)' to the event bus.
		*
		* @tparam _t The type of instance.
		* @param instance An instance of '_t' to be subscribed to the event bus.
		*/
		template<typename _t>
		void subscribe(std::shared_ptr<_t> instance) {
			using expanded_events = int[];
			expanded_events{
				(event_bus<_event_t>::subscribe(instance), 0),
				(event_bus<_others_t>::subscribe(instance), 0)...
			};
		}

		/**
		* @breif Subscribes a free function to event bus.
		*
		* Subscribes a free function to the event bus. The function
		* can have any name, but still needs to return 'void' and 
		* take a 'const Event&' as a parameter.
		*
		* @tparam __event_t The type of event that an event handler takes as a parameter.
		* @tparam _function The pointer to a free event handler.
		*/
		template<typename __event_t, void(*_function)(const __event_t&)>
		void subscribe() {
			event_bus<__event_t>::template subscribe<_function>();
		}

		/**
		* @breif Unsubscribes a class instance from the event bus.
		*
		* Unsubscribes all public member functions of the signature
		* 'void _t::process(const Event&)' from the event bus.
		*
		* @tparam _t The type of instance.
		* @param instance An instance of '_t' to be subscribed to the event bus.
		*/
		template<typename _t>
		void unsubscribe(std::shared_ptr<_t> instance) {
			using expanded_events = int[];
			expanded_events{
				(event_bus<_event_t>::template unsubscribe(instance), 0),
				(event_bus<_others_t>::template unsubscribe(instance), 0)...
			};
		}

		/**
		* @breif Unsubscribes a free function from event bus.
		*
		* Unsubscribes a free function from the event bus.
		*
		* @tparam __event_t The type of event that an event handler takes as a parameter.
		* @tparam _function The pointer to a free event handler
		*/
		template<typename __event_t, void(*_function)(const __event_t&)>
		void unsubscribe() {
			event_bus<__event_t>::template unsubscribe<_function>();
		}

		/**
		* @breif Publishes an event to the bus with the specified arguments.
		*
		* Publishes an event to all of the event handlers that can 
		* handle the type of event.
		*
		* @tparam __event_t The type of event to be published.
		* @tparam _args_t The parameters to construct '__event_t'.
		* @param args The arguments to construct '__event_t'.
		*/
		template<typename __event_t, typename... _args_t>
		void publish(_args_t&&... args) {
			event_bus<__event_t>::template publish(args...);
		}
	};

	/**
	* @breif Event bus specilization for a sigle event type.
	*
	* The event bus allows member functions to be added and removed from its
	* signal. Classes must publicly expose functions of the
	* signature 'void _t::process(const Event&)' to be added. Free functions can
	* also be added, but do not need any specific name.
	*
	* When events are published, the event bus notifies all of the event
	* handlers connected to the bus with the specified event type and parameters.
	*
	* @tparam _event_t The type of event managed by this bus.
	*/
	template<typename _event_t>
	class event_bus<_event_t> : signal<const _event_t&> {
	private:
		using signal_type = signal<const _event_t&>;

		template<typename T>
		auto subscribe_func(std::shared_ptr<T> instance)
			-> decltype(std::declval<T>().process(std::declval<_event_t>()), void()) {
			signal_type::template subscribe<T, &T::process>(instance);
		}

		template<typename T>
		auto unsubscribe_func(std::shared_ptr<T> instance)
			-> decltype(std::declval<T>().process(std::declval<_event_t>()), void()) {
			signal_type::template unsubscribe<T, &T::template process>(instance);
		}

		auto subscribe_func(...) -> void {};
		auto unsubscribe_func(...) -> void {};
	public:
		/**
		* @breif Publishes an event to the bus with the specified arguments.
		*
		* Publishes an event, constructed from the specified arguments, to all of
		* the event handlers that can handle the spcified type of event.
		*
		* @tparam __event_t The type of event to be published.
		* @param args The arguments to construct '__event_t' with.
		*/
		template<typename... __event_t>
		void publish(__event_t&&... args) {
			signal_type::template publish({ args... });
		}

		/**
		* @breif Subscribes a free function to event bus.
		*
		* Subscribes a free function to the event bus. The function
		* can have any name, but still needs to return 'void' and
		* take a 'const Event&' as a parameter.
		*
		* @tparam _function The pointer to a free event handler.
		*/
		template<void(*_function)(const _event_t&)>
		void subscribe() {
			signal_type::template subscribe<_function>();
		}

		/**
		* @breif Subscribes a class instance to the event bus.
		*
		* Subscribes a public member function of the signature
		* 'void _t::process(const Event&)' to the event bus.
		*
		* @tparam _t The type of instance.
		* @param instance An instance of 'ClasType' to be subscribed to the event bus.
		*/
		template<typename _t>
		void subscribe(std::shared_ptr<_t> instance) {
			subscribe_func(instance);
		}

		/**
		* @breif Unsubscribes a free function from event bus.
		*
		* Unsubscribes a free function from the event bus. The function
		* can have any name, but still needs to return 'void' and
		* take a 'const Event&' as a parameter.
		*
		* @tparam _function Function pointer to the free event handler.
		*/
		template<void(*_function)(const _event_t&)>
		void unsubscribe() {
			signal_type::template unsubscribe<_function>();
		}

		/**
		* @breif Unsubscribes a class instance from the event bus.
		*
		* Unsubscribes the public member function of the signature
		* 'void _t::process(const Event&)' from the event bus.
		*
		* @tparam _t Type of instance.
		* @param instance Instance of _t to be subscribed to the event bus.
		*/
		template<typename _t>
		void unsubscribe(std::shared_ptr<_t> instance) {
			unsubscribe_func(instance);
		}
	};
}
