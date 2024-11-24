#include "profiler_service.hpp"
#include <spritemancer.h>

namespace engine
{
	namespace
	{
		static core::ctimer S_TIMER;
		constexpr float S_UPDATE_INTERVAL = 5.0f;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool cprofiler_service::init()
	{
#if PROFILE_ENABLE
		core::profile::cpu::set_default_aggregator();
		core::profile::memory::set_default_aggregator();
		sm::profile::gpu::set_default_aggregator();
#endif
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprofiler_service::~cprofiler_service()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cprofiler_service::on_start()
	{
		//- If we have profile enabled, make sure that we have created aggregators
#if PROFILE_ENABLE
		CORE_ASSERT(core::profile::memory::get_aggregator(), "Invalid operation. No memory profiler set!");
		CORE_ASSERT(core::profile::cpu::get_aggregator(), "Invalid operation. No CPU profiler set!");
		CORE_ASSERT(sm::profile::gpu::get_aggregator(), "Invalid operation. No GPU profiler set!");
#endif
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprofiler_service::on_shutdown()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprofiler_service::on_update(float dt)
	{
		if (!S_TIMER.started())
		{
			S_TIMER.start(); return;
		}

		//- Update data every so many seconds
		if (S_TIMER.secs() > S_UPDATE_INTERVAL)
		{
			if (auto aggregator = core::profile::memory::get_aggregator(); aggregator)
			{
				aggregator->update();
			}
			if (auto aggregator = core::profile::cpu::get_aggregator(); aggregator)
			{
				aggregator->update();
			}
			if (auto aggregator = sm::profile::gpu::get_aggregator(); aggregator)
			{
				aggregator->update();
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprofiler_service::force_update()
	{
		memory_force_update();
		cpu_force_update();
		gpu_force_update();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprofiler_service::memory_force_update()
	{
		if (auto aggregator = core::profile::memory::get_aggregator(); aggregator)
		{
			aggregator->update();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpu_function_t cprofiler_service::cpu_function_data(stringview_t function_name) const
	{
		if (auto aggregator = core::profile::cpu::get_aggregator(); aggregator)
		{
			return aggregator->function_data(function_name);
		}
		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpu_function_data_t cprofiler_service::cpu_function_data() const
	{
		if (auto aggregator = core::profile::cpu::get_aggregator(); aggregator)
		{
			return aggregator->function_data();
		}
		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprofiler_service::gpu_force_update()
	{
		if (auto aggregator = sm::profile::gpu::get_aggregator(); aggregator)
		{
			aggregator->update();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	gpu_stats_t cprofiler_service::gpu_stats() const
	{
		if (auto aggregator = sm::profile::gpu::get_aggregator(); aggregator)
		{
			return aggregator->stats();
		}
		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	gpu_drawcall_data_t cprofiler_service::gpu_drawcall_data() const
	{
		if (auto aggregator = sm::profile::gpu::get_aggregator(); aggregator)
		{
			return aggregator->drawcall_data();
		}
		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	memory_stats_t cprofiler_service::memory_stats() const
	{
		if (auto aggregator = core::profile::memory::get_aggregator(); aggregator)
		{
			return aggregator->stats();
		}
		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprofiler_service::cpu_force_update()
	{
		if (auto aggregator = core::profile::cpu::get_aggregator(); aggregator)
		{
			return aggregator->update();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpu_stats_t cprofiler_service::cpu_stats() const
	{
		if (auto aggregator = core::profile::cpu::get_aggregator(); aggregator)
		{
			return aggregator->stats();
		}
		return {};
	}

} //- engine

RTTR_REGISTRATION
{
	using namespace engine;

	rttr::cregistrator<cprofiler_service, rttr::detail::as_raw_pointer>("cprofiler_service")
		;
}