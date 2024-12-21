#pragma once
#include "core_config.hpp"

namespace core
{
	//- Class wrapping a future type. If copied from another future then both are waiting for the same result.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	class cfuture_type
	{
	public:
		cfuture_type(const std::future<TType>& task) : m_task(task.share()), m_status(future_status_pending) {}
		cfuture_type(const std::shared_future<TType>& task) : m_task(task), m_status(future_status_pending) {}
		cfuture_type(const cfuture_type<TType>& other) : m_task(other.m_task), m_status(other.m_status) {}
		cfuture_type() = default;
		~cfuture_type() = default;

		bool ready();
		const TType& wait();
		const TType& get() { return wait(); }

	private:
		std::shared_future<TType> m_task;
		future_status m_status = future_status_none;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	const TType& core::cfuture_type<TType>::wait()
	{
		return m_task.get();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	bool core::cfuture_type<TType>::ready()
	{
		using namespace std::chrono_literals;

		switch (m_task.wait_for(1us))
		{
		case std::future_status::ready:
		{
			m_status = future_status_ready;
			break;
		}
		case std::future_status::deferred:
		{
			m_status = future_status_deferred;
			break;
		}
		default:
		case std::future_status::timeout:
		{
			m_status = future_status_pending;
			break;
		}
		}
		return m_status == future_status_ready;
	}

} //- core