#pragma once

#include "iw/util/queue/blocking_queue.h"
#include "iw/log/logger.h"
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
//#include <string>

//#define TASK_IN(t, f)							\
//	float time = iw::TotalTime();					\
//	float delay = t;							\
//	Task->coroutine(							\
//		[=]()								\
//	{										\
//		bool done = iw::TotalTime() - time > delay;	\
//		if (done)	f;	                              \
//		return done;							\
//	});										\

namespace iw {
namespace util {
	class thread_pool {
	private:
		struct func_t {
			std::function<void()> func;
			//std::string name;
			bool poison = false;
		};

		std::vector<std::thread> m_pool;
		iw::blocking_queue<func_t> m_queue;

		std::mutex m_mutex_coroutine;
		std::vector<std::function<bool()>> m_coroutines; // main thread tasks
		std::vector<std::function<void()>> m_defered;    // ^

	public:
		thread_pool(
			unsigned threads)
			//: m_poison(detail::poison)
		{
			m_pool.resize(threads);
		}

		~thread_pool() {
			shutdown();
		}

		void init() {
			for (std::thread& thread : m_pool) {
				thread = std::thread([&]() {
					while (true) {
						func_t/*&*/ func = m_queue.pop(); // was reference the problem?

						if (func.poison) {
							break;
						}

						//LOG_DEBUG << "Called function " << func.name;

						if (!func.func) {
							LOG_ERROR << "Tried to call queued function in thread pool but it was missing!";
						}

						func.func();
					}
				});
			}
		}

		void shutdown() {
			for (std::thread& thread : m_pool) {
				func_t t {
					{},
					//"",
					true // stop thread
				};

				m_queue.push(t);
			}

			for (std::thread& thread : m_pool) {
				thread.join();
			}
		}

		void queue(
			std::function<void()> func/*,
			std::string& name*/)
		{
			func_t t {
				func,
				//name,
				false
			};

			m_queue.push(t);
		}

		void step_coroutines()
		{
			std::vector<std::function<bool()>> copy;
			std::vector<std::function<void()>> copy2;
			{
				std::unique_lock lock(m_mutex_coroutine);
				copy  = m_coroutines;
				copy2 = m_defered;
			}

			for (size_t i = 0; i < copy.size(); i++)
			{
				if (copy.at(i)())
				{
					std::unique_lock lock(m_mutex_coroutine);
					m_coroutines.erase(m_coroutines.begin() + i);
				}
			}

			for (size_t i = 0; i < copy2.size(); i++)
			{
				copy2.at(i)();
			}

			{
				std::unique_lock lock(m_mutex_coroutine);
				m_defered.clear();
			}
		}

		void coroutine(
			std::function<bool()> func)
		{
			std::unique_lock lock(m_mutex_coroutine);
			m_coroutines.push_back(func);
		}

		void defer(
			std::function<void()> func)
		{
			std::unique_lock lock(m_mutex_coroutine);
			m_defered.push_back(func);
		}

		template<
			typename _container,
			typename _func>
		void foreach(
			_container& container,
			_func&& func)
		{
			std::mutex mutex;
			std::condition_variable cond;

			size_t remaining = container.size();

			for (auto index = 0; index < container.size(); index++)
			{
				queue([&, func, index]() // need to copy func?
				{
					func(index);

					std::unique_lock lock(mutex);
					remaining -= 1;
					cond.notify_one();
				});
			}

			std::unique_lock lock(mutex);
			cond.wait(lock, [&]() { return remaining == 0; });
		}

		template<
			typename _container,
			typename _preThreadFunc,
			typename _func>
		void foreach(
			_container& container,
			_preThreadFunc&& preThreadFunc,
			_func&& func)
		{
			std::mutex mutex;
			std::condition_variable cond;

			size_t remaining = container.size();
			auto index = 0;

			for (auto& element : container)
			{
				auto preThread = preThreadFunc(element);

				queue([&, func, index, preThread]() // need to copy func?
				{
					func(index, preThread);

					std::unique_lock lock(mutex);
					remaining -= 1;
					cond.notify_one();
				});

				index += 1;
			}

			std::unique_lock lock(mutex);
			cond.wait(lock, [&]() { return remaining == 0; });
		}
	};
}

	using namespace util;
}
