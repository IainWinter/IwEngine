#pragma once

#include "iw/util/queue/blocking_queue.h"
#include "iw/log/logger.h"
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

#include <chrono>

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

		using my_time = long long;

		my_time get_time() {
			using namespace std::chrono;
			return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		}

		std::vector<std::thread> m_pool;
		iw::blocking_queue<func_t> m_queue;

		std::mutex m_mutex_coroutine;
		std::vector<                           std::function<bool()>>  m_coroutines; // main thread tasks
		std::vector<std::tuple<my_time, float, std::function<void()>>> m_defered;    // ^

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
			// copy to local because tasks could add more

			std::vector<                           std::function<bool()>>  copy_coroutines;
			std::vector<std::tuple<my_time, float, std::function<void()>>> copy_defered;
			{
				std::unique_lock lock(m_mutex_coroutine);
				copy_coroutines = m_coroutines;
				copy_defered    = m_defered;
			}

			for (size_t i = 0; i < copy_coroutines.size(); i++)
			{
				if (copy_coroutines.at(i)())
				{
					copy_coroutines.erase(copy_coroutines.begin() + i); // sucks, lots of moves
				}
			}

			for (size_t i = 0; i < copy_defered.size(); i++)
			{
				auto& [begin, delay, func] = copy_defered.at(i);

				if (get_time() - begin > (my_time)(delay * 1000.f))
				{
					func();
					copy_defered.erase(copy_defered.begin() + i);
				}
			}

			// move back
			{
				std::unique_lock lock(m_mutex_coroutine);
				m_coroutines = std::move(copy_coroutines);
				m_defered    = std::move(copy_defered);
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
			delay(-1.f, func);
		}

		void delay(
			float howLong,
			std::function<void()> func)
		{
			std::unique_lock lock(m_mutex_coroutine);
			m_defered.emplace_back(get_time(), howLong, func);
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
