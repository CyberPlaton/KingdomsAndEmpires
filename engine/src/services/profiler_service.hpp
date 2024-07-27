#pragma once
#include <core.h>

namespace engine
{
	//- Service allowing access to aggregated stats from profilers. Can also further refine the data and ease reasoning about it.
	//- The raw profiling data is gathered in 'core' library using various aggregators.
	//------------------------------------------------------------------------------------------------------------------------
	class cprofiler_service final : public core::cservice
	{
	public:
		static bool init();

		cprofiler_service() = default;
		~cprofiler_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float dt) override final;

		core::profile::memory::smemory_stats memory_stats() const;
		vector_t<core::profile::cpu::scpu_stats> cpu_stats() const;

	private:
		RTTR_ENABLE(core::cservice);
	};

} //- engine