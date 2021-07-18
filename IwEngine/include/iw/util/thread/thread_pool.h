#pragma once

#include "iw/util/queue/blocking_queue.h"
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
//#include <string>

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

		template<
			typename _container,
			typename _preThreadFunc,
			typename _func>
		void foreach(
			const _container& container,
			_preThreadFunc&& preThreadFunc,
			_func&& func)
		{
			std::mutex mutex;
			std::condition_variable cond;

			int remaining = container.size();
			int index = -1; // adds 1 before call

			for (const auto& element : container)
			{
				index += 1;
				auto preThread = preThreadFunc(element);

				queue([&, func, index, preThread]() // need to copy func?
				{
					func(index, preThread);

					std::unique_lock lock(mutex);
					remaining -= 1;
					cond.notify_one();
				});
			}

			std::unique_lock lock(mutex);
			cond.wait(lock, [&]() { return remaining == 0; });
		}
	};
}

	using namespace util;
}
