#pragma once

#include <iostream>
#include "signal.h"

namespace iwevents {
	/*
	Fixes doxygen problem where it does not detect that event_bus<Event, Other...> 
	inherits from event_bus<Event> and event_bus<Others>....
	*/
	template<typename... Events>
	class event_bus;

	/**
	* @breif Event bus specilization for many event types.
	*
	* The event bus allows member functions to be added and removed from many
	* signals at once. Classes must publicly expose functions of the
	* signature 'void ClassType::process(const Event&)' to be added. Free functions can
	* also be added, but do not need any specific name.
	*
	* When events are published, the event bus notifies all of the event
	* handlers connected to the bus with the specified event type and arguments.
	*
	* @param Event The list of events managed by the event bus.
	*/
	template<typename Event, typename... Others>
	class event_bus<Event, Others...> : event_bus<Event>, event_bus<Others>... {
	public:
		/**
		* @breif Subscribes a class instance to the event bus.
		*
		* Subscribes all public member functions of the signature 
		* 'void ClassType::process(const Event&)' to the event bus.
		*
		* @param ClassType The type of instance.
		* @param instance An instance of 'ClassType' to be subscribed to the event bus.
		*/
		template<typename ClassType>
		void subscribe(std::shared_ptr<ClassType> instance) {
			using expanded_events = int[];
			expanded_events{
				(event_bus<Event>::subscribe(instance), 0),
				(event_bus<Others>::subscribe(instance), 0)...
			};
		}

		/**
		* @breif Subscribes a free function to event bus.
		*
		* Subscribes a free function to the event bus. The function
		* can have any name, but still needs to return 'void' and 
		* take a 'const Event&' as a parameter.
		*
		* @param EventType The type of event that an event handler takes as a parameter.
		* @param Function The pointer to a free event handler.
		*/
		template<typename EventType, void(*Function)(const EventType&)>
		void subscribe() {
			event_bus<EventType>::template subscribe<Function>();
		}

		/**
		* @breif Unsubscribes a class instance from the event bus.
		*
		* Unsubscribes all public member functions of the signature
		* 'void ClassType::process(const Event&)' from the event bus.
		*
		* @param ClassType The type of instance.
		* @param instance An instance of 'ClassType' to be subscribed to the event bus.
		*/
		template<typename ClassType>
		void unsubscribe(std::shared_ptr<ClassType> instance) {
			using expanded_events = int[];
			expanded_events{
				(event_bus<Event>::template unsubscribe(instance), 0),
				(event_bus<Others>::template unsubscribe(instance), 0)...
			};
		}

		/**
		* @breif Unsubscribes a free function from event bus.
		*
		* Unsubscribes a free function from the event bus.
		*
		* @param EventType The type of event that an event handler takes as a parameter.
		* @param Function The pointer to a free event handler
		*/
		template<typename EventType, void(*Function)(const EventType&)>
		void unsubscribe() {
			event_bus<EventType>::template unsubscribe<Function>();
		}

		/**
		* @breif Publishes an event to the bus with the specified arguments.
		*
		* Publishes an event to all of the event handlers that can 
		* handle the type of event.
		*
		* @param EventType The type of event to be published.
		* @param Args The parameters to construct 'EventType'.
		* @param args The arguments to construct 'EventType'.
		*/
		template<typename EventType, typename... Args>
		void publish(Args&&... args) {
			event_bus<EventType>::template publish(args...);
		}
	};

	/**
	* @breif Event bus specilization for a sigle event type.
	*
	* The event bus allows member functions to be added and removed from its
	* signal. Classes must publicly expose functions of the
	* signature 'void ClassType::process(const Event&)' to be added. Free functions can
	* also be added, but do not need any specific name.
	*
	* When events are published, the event bus notifies all of the event
	* handlers connected to the bus with the specified event type and parameters.
	*
	* @param Event The type of event managed by this bus.
	*/
	template<typename Event>
	class event_bus<Event> : signal<const Event&> {
	private:
		using signal_type = signal<const Event&>;

		template<typename T>
		auto subscribe_func(std::shared_ptr<T> instance)
			-> decltype(std::declval<T>().process(std::declval<Event>()), void()) {
			signal_type::template subscribe<T, &T::process>(instance);
		}

		template<typename T>
		auto unsubscribe_func(std::shared_ptr<T> instance)
			-> decltype(std::declval<T>().process(std::declval<Event>()), void()) {
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
		* @param EventType The type of event to be published.
		* @param args The arguments to construct 'EventType' with.
		*/
		template<typename... EventType>
		void publish(EventType&&... args) {
			signal_type::template publish({ args... });
		}

		/**
		* @breif Subscribes a free function to event bus.
		*
		* Subscribes a free function to the event bus. The function
		* can have any name, but still needs to return 'void' and
		* take a 'const Event&' as a parameter.
		*
		* @param Function The pointer to a free event handler.
		*/
		template<void(*Function)(const Event&)>
		void subscribe() {
			signal_type::template subscribe<Function>();
		}

		/**
		* @breif Subscribes a class instance to the event bus.
		*
		* Subscribes a public member function of the signature
		* 'void ClassType::process(const Event&)' to the event bus.
		*
		* @param ClassType The type of instance.
		* @param instance An instance of 'ClasType' to be subscribed to the event bus.
		*/
		template<typename ClassType>
		void subscribe(std::shared_ptr<ClassType> instance) {
			subscribe_func(instance);
		}

		/**
		* @breif Unsubscribes a free function from event bus.
		*
		* Unsubscribes a free function from the event bus. The function
		* can have any name, but still needs to return 'void' and
		* take a 'const Event&' as a parameter.
		*
		* @param Function Function pointer to the free event handler.
		*/
		template<void(*Function)(const Event&)>
		void unsubscribe() {
			signal_type::template unsubscribe<Function>();
		}

		/**
		* @breif Unsubscribes a class instance from the event bus.
		*
		* Unsubscribes the public member function of the signature
		* 'void ClassType::process(const Event&)' from the event bus.
		*
		* @param ClassType Type of instance.
		* @param instance Instance of ClassType to be subscribed to the event bus.
		*/
		template<typename ClassType>
		void unsubscribe(std::shared_ptr<ClassType> instance) {
			unsubscribe_func(instance);
		}
	};
}