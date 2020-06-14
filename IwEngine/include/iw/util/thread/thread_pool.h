#pragma once

#include "iw/util/queue/blocking_queue.h"
#include <vector>
#include <thread>
#include <functional>

namespace iw {
namespace util {
	class thread_pool {
	private:
		struct func_t {
			std::function<void()> func;
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
						func_t& func = m_queue.pop();

						if (func.poison) {
							break;
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
					true // stop thread
				};

				m_queue.push(t);
			}

			for (std::thread& thread : m_pool) {
				thread.join();
			}
		}

		void queue(
			std::function<void()> func)
		{
			func_t t {
				func,
				false
			};

			m_queue.push(t);
		}
	};
}

	using namespace util;
}
