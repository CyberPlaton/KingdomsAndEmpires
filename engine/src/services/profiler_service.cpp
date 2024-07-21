#include "profiler_service.hpp"

namespace engine
{
	namespace
	{
		static core::ctimer S_TIMER;
		constexpr float S_UPDATE_INTERVAL = 5.0f;

	} //- unnamed

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
	core::profile::cpu::scpu_stats cprofiler_service::cpu_stats() const
	{
		if (auto aggregator = core::profile::cpu::get_aggregator(); aggregator)
		{
			return aggregator->stats();
		}
		return {};
	}

} //- engine