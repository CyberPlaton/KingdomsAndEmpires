#pragma once
#include <core.h>

namespace engine
{
	namespace detail
	{
		template<typename TCallable>
		using packaged_task_t = std::packaged_task<TCallable>;

	} //- detail

	//-
	//------------------------------------------------------------------------------------------------------------------------
	class cthread_service final : public core::cservice
	{
	public:
		static unsigned hardware_threads();

		cthread_service() = default;
		~cthread_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		template<typename TCallable>
		decltype(auto) push_mainthread_job(TCallable&& callable);

		template<typename TCallable>
		decltype(auto) push_background_job(const std::string& name, TCallable&& callable);

		inline tf::Future<void> push_background_taskflow(tf::Taskflow& taskflow){return m_bg->run(taskflow);}

		template<typename TCallable>
		decltype(auto) push_foreground_job(const std::string& name, TCallable&& callable);

		inline tf::Future<void> push_foreground_taskflow(tf::Taskflow& taskflow){return m_fg->run(taskflow);}

	private:
		using mainthread_job_t = std::function<void()>;

		core::cmutex m_mutex;

		vector_t<mainthread_job_t> m_mainthread_jobs;

		ptr_t<tf::Executor> m_bg;
		ptr_t<tf::Executor> m_fg;

		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TCallable>
	decltype(auto) cthread_service::push_mainthread_job(TCallable&& callable)
	{
		auto task = detail::packaged_task_t<TCallable>(std::forward<TCallable>(callable));

		{
			core::cscope_mutex m(m_mutex);

			//- store a task wrapper that executes the callable function
			m_mainthread_jobs.push_back([&]()
				{
					(*task)();
				});
		}

		return task.get_future();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TCallable>
	decltype(auto) cthread_service::push_background_job(const std::string& name, TCallable&& callable)
	{
		return m_bg->async(name, std::forward<TCallable>(callable));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TCallable>
	decltype(auto) cthread_service::push_foreground_job(const std::string& name, TCallable&& callable)
	{
		return m_fg->async(name, std::forward<TCallable>(callable));
	}

} //- engine

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cthread_service)
	{
		rttr::registration::class_<cthread_service>("cthread_service")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;

		rttr::default_constructor<cthread_service>();
	}

} //- engine