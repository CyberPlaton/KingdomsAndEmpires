#pragma once
#include "sm_config.hpp"

namespace sm
{
	namespace profile::gpu
	{
		class iaggregator;
		using aggregator_ref_t = ref_t<iaggregator>;

		//------------------------------------------------------------------------------------------------------------------------
		struct sdrawcall_data
		{
			uint64_t m_vertices = 0;
			float m_time = 0.0f;	//- microsecs
			const char* m_name = nullptr;
		};

		//------------------------------------------------------------------------------------------------------------------------
		class cscoped_drawcall final
		{
		public:
			cscoped_drawcall(uint64_t vertices, const char* name);
			~cscoped_drawcall();

		private:
			sdrawcall_data m_data;
			core::ctimer m_timer;
		};

		//- As long as we use raylib the stats are meant to be quite inaccurate, giving only a general idea of resources used.
		//------------------------------------------------------------------------------------------------------------------------
		struct saggregated_drawcall_data
		{
			uint64_t m_drawcalls = 0;
			uint64_t m_vertices = 0;

			uint64_t m_textures_count = 0;
			uint64_t m_shaders_count = 0;
			uint64_t m_rendertargets_count = 0;

			uint64_t m_textures_bytes = 0;
			uint64_t m_shaders_bytes = 0;
			uint64_t m_rendertargets_bytes = 0;

			float m_drawing_time_total = 0.0f;
			float m_drawing_time_mean = 0.0f;
		};

		//- Basic information about GPU. Note: Currently cores and clock speed seem not to work correctly.
		//------------------------------------------------------------------------------------------------------------------------
		struct sgpu_stats
		{
			string_t m_model_vendor_driver;

			int64_t m_cores = 0;
			int64_t m_clock_speed = 0;	//- MHz
			int64_t m_memory = 0;		//- Bytes
		};

		//- Interface class to gather stats on GPU memory usage and allocations.
		//------------------------------------------------------------------------------------------------------------------------
		class iaggregator
		{
		public:
			virtual ~iaggregator() = default;

			virtual vector_t<sgpu_stats>		stats() = 0;
			virtual saggregated_drawcall_data	drawcall_data() = 0;
			virtual void						update() = 0;
			virtual void						push(gpu::sdrawcall_data&& data) = 0;
		};

		void set_aggregator(aggregator_ref_t object);
		void set_default_aggregator();
		aggregator_ref_t get_aggregator();

	} //- profile::gpu

	namespace profile
	{
		//- Note: do not use directly, use profiler service instead, he will have data if PROFILE_ENABLE was defined and he
		//- also does necessary initialization.
		//------------------------------------------------------------------------------------------------------------------------
		class cprofiler final
		{
		public:
			//- GPU
			static void								push(gpu::sdrawcall_data&& data);
			static gpu::saggregated_drawcall_data	drawcall_data();
		};

	} //- profile

} //- sm