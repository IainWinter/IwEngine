#include "Core.h"
#include <thread>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace iw {
namespace Engine {
	template<
		typename _f>
	class Task {
	private:
		std::thread m_thread;
		std::mutex  m_mutex;
		std::condition_variable m_condition;
		std::function<_f> m_functor;
		std::atomic<bool> m_run;
		std::atomic<bool> m_joining;

	public:
		Task() = default;

		Task(
			std::function<_f> functor)
			: m_functor(functor)
			, m_run(false)
			, m_joining(false)
		{
			m_thread = std::thread([&] {
				while (true) {
					std::unique_lock<std::mutex> lock(m_mutex);
					m_condition.wait(lock, [&]() {
						return m_run == true || m_joining == true;
					});

					if (m_joining)
						break;

					m_functor();

					m_run = false;
					m_condition.notify_all();
				}
			});
		}

		Task(const Task&) = delete;
		Task(Task&&) = delete;

		~Task() {
			if (m_thread.joinable()) {
				std::unique_lock<std::mutex> lock(m_mutex);

				m_joining = true;
				m_condition.notify_all();

				lock.unlock();
				m_thread.join();
			}
		}

		Task& operator=(const Task&) = delete;
		Task& operator=(Task&&) = delete;

		void Run() {
			std::unique_lock<std::mutex> lock(m_mutex);

			m_run = true;
			m_condition.notify_all();

			lock.unlock();
		}

		void Stop() {
			std::unique_lock<std::mutex> lock(m_mutex);

			m_joining = true;
			m_condition.notify_all();

			lock.unlock();
		}

		void Wait() {
			std::unique_lock<std::mutex> lock(m_mutex);
			m_condition.wait(lock, [&]() {
				return m_run == false;
			});
		}
	};
}

	using namespace Engine;
}
