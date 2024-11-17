#include "thread_service.hpp"

namespace engine
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		void set_thread_names(vector_t<std::thread>& threads, stringview_t prefix)
		{
			for (auto i = 0u; i < threads.size(); ++i)
			{
				const auto name = fmt::format("{}{}", prefix, i);
				auto& thread = threads[i];

				try
				{
#if CORE_PLATFORM_WINDOWS
					//- stupid windows
					constexpr auto C_MAX_LENGTH = 256;
					wchar_t wide_name[C_MAX_LENGTH];
					if (const auto result = MultiByteToWideChar(CP_UTF8, 0, name.data(), SCAST(int, name.length()), wide_name, C_MAX_LENGTH - 1); result != 0)
					{
						wide_name[result] = 0;
						SetThreadDescription(thread.native_handle(), wide_name);
					}
#elif CORE_PLATFORM_LINUX
					pthread_setname_np(thread.native_handle(), name.data());
#elif CORE_PLATFORM_OSX
                    //- nothing to be done here. Names are set in threads individual context
#endif
				}
				catch (const std::exception& err)
				{
					log_error(fmt::format("Failed to set name for thread '{}' to '{}'. Reason: '{}'",
						std::hash<std::thread::id>{}(thread.get_id()), name, err.what()));
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		class cworker_interface final : public tf::WorkerInterface
		{
		public:
			cworker_interface(stringview_t prefix, unsigned thread_count) :
				m_prefix(prefix)
			{
				m_threads.reserve(thread_count);
			}

			~cworker_interface() = default;

			void scheduler_prologue(tf::Worker& worker) override final
			{
				const auto id = worker.id();
				const auto name = fmt::format("{}{}", m_prefix, id);

				//- called before worker enters a loop
                //- set name for each thread individually from their context
#if CORE_PLATFORM_OSX
                pthread_setname_np(name.c_str());
#endif
#if TRACY_ENABLE
                //- set name for tracy if we are profiling
                TracyCSetThreadName(name.c_str());
#endif

				m_threads[id] = std::this_thread::get_id();

				log_debug(fmt::format("\tworker '{}' entered loop...", name));
			}

			void scheduler_epilogue(tf::Worker& worker, std::exception_ptr ptr) override final
			{
				//- called when worker exits the loop
				const auto id = worker.id();
				const auto name = fmt::format("{}{}", m_prefix, id);

				algorithm::erase_first(m_threads, id);

				log_debug(fmt::format("\tworker '{}' exited loop...", name));
			}

		private:
			umap_t<size_t, std::thread::id> m_threads;
			const std::string m_prefix;
		};

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cthread_service::hardware_threads()
	{
		return std::thread::hardware_concurrency();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cthread_service::~cthread_service()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cthread_service::on_start()
	{
		const auto bg_thread_prefix = "Background Worker #";
		const auto fg_thread_prefix = "Foreground Worker #";
		const auto thread_count = hardware_threads();
		const auto bg_count = thread_count / 2;
		const auto fg_count = thread_count / 2;

		m_bg = std::make_unique<tf::Executor>(bg_count, std::make_shared<cworker_interface>(bg_thread_prefix, bg_count));
		m_fg = std::make_unique<tf::Executor>(fg_count, std::make_shared<cworker_interface>(fg_thread_prefix, fg_count));

		set_thread_names(m_bg->worker_threads(), bg_thread_prefix);
		set_thread_names(m_fg->worker_threads(), fg_thread_prefix);

		log_info(fmt::format("Starting thread service with '{}' background workers and '{}' foreground workers",
			bg_count, fg_count));

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cthread_service::on_shutdown()
	{
		m_fg.release();
		m_bg.release();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cthread_service::on_update(float)
	{
		vector_t<mainthread_job_t> jobs;

		{
			core::cscope_mutex m(m_mutex);

			algorithm::swap(jobs, m_mainthread_jobs);

			m_mainthread_jobs.clear();
		}

		for (const auto& job : jobs)
		{
			job();
		}
	}

} //- engine

RTTR_REGISTRATION
{
	using namespace engine;

	rttr::cregistrator<cthread_service, rttr::detail::as_raw_pointer>("cthread_service")
		;
}
