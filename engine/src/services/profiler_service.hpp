#pragma once
#include <core.h>
#include <spritemancer.h>

namespace engine
{
	using gpu_stats_t = vector_t<sm::profile::gpu::sgpu_stats>;
	using gpu_drawcall_data_t = sm::profile::gpu::saggregated_drawcall_data;

	using cpu_stats_t = vector_t<core::profile::cpu::scpu_stats>;
	using cpu_function_t = core::profile::cpu::saggregated_function_data;
	using cpu_function_data_t = umap_t<const char*, cpu_function_t>;

	using memory_stats_t = core::profile::memory::smemory_stats;

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
		void force_update();

		//- Memory
		void memory_force_update();
		memory_stats_t memory_stats() const;

		//- CPU
		void cpu_force_update();
		cpu_stats_t cpu_stats() const;
		cpu_function_t cpu_function_data(stringview_t function_name) const;
		cpu_function_data_t cpu_function_data() const;

		//- GPU
		void gpu_force_update();
		gpu_stats_t gpu_stats() const;
		gpu_drawcall_data_t gpu_drawcall_data() const;

	private:
		RTTR_ENABLE(core::cservice);
	};

} //- engine