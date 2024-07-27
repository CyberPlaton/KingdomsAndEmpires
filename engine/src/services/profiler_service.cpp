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
		core::profile::cpu::set_default_aggregator();
		core::profile::memory::set_default_aggregator();
		sm::profile::gpu::set_default_aggregator();

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprofiler_service::~cprofiler_service()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cprofiler_service::on_start()
	{
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

		//- Update data every so often
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
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::profile::memory::smemory_stats cprofiler_service::memory_stats() const
	{
		if (auto aggregator = core::profile::memory::get_aggregator(); aggregator)
		{
			return aggregator->stats();
		}
		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	vector_t<core::profile::cpu::scpu_stats> cprofiler_service::cpu_stats() const
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