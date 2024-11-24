#pragma once
#include "core_config.hpp"
#include "core_timer.hpp"

namespace core
{
	namespace profile
	{
		namespace memory
		{
			class iaggregator;
			using aggregator_ref_t = ref_t<iaggregator>;

			//- One entry in the memory stats table. Consists of the value and a description of it.
			//------------------------------------------------------------------------------------------------------------------------
			struct smemory_stats_entry final
			{
				uint64_t m_value;
				stringview_t m_desc;
			};

			//- Gathered stats on memory usage and the like. Ready to go.
			//------------------------------------------------------------------------------------------------------------------------
			struct smemory_stats final
			{
				//- General
				smemory_stats_entry m_total_reserved_memory;
				smemory_stats_entry m_peak_total_reserved_memory;
				smemory_stats_entry m_total_used_memory;
				smemory_stats_entry m_peak_total_used_memory;

				smemory_stats_entry m_elapsed_process_time;
				smemory_stats_entry m_elapsed_user_code_time;
				smemory_stats_entry m_elapsed_system_code_time;

				smemory_stats_entry m_total_resident_set_size;
				smemory_stats_entry m_peak_total_resident_set_size;

				//- Allocations
				smemory_stats_entry m_normal_allocations_bytes;
				smemory_stats_entry m_large_allocations_bytes;
				smemory_stats_entry m_huge_allocations_bytes;

				//- Operations/Counters
				smemory_stats_entry m_modifications_count;
				smemory_stats_entry m_reset_count;
				smemory_stats_entry m_purge_count;
				smemory_stats_entry m_system_call_memory_map_count;
				smemory_stats_entry m_system_call_memory_commit_count;
				smemory_stats_entry m_free_block_search_operations_count;

				//- Segementations/Paging
				smemory_stats_entry m_segments_count;
				smemory_stats_entry m_pages_count;
			};

			//- Interface for a class to gather stats on memory and return them back.
			//------------------------------------------------------------------------------------------------------------------------
			class iaggregator
			{
			public:
				virtual ~iaggregator() = default;

				virtual smemory_stats	stats() = 0;
				virtual void			update() = 0;
			};

			void set_aggregator(aggregator_ref_t object);
			void set_default_aggregator();
			aggregator_ref_t get_aggregator();

		} //- memory

		namespace cpu
		{
			class iaggregator;
			using aggregator_ref_t = ref_t<iaggregator>;

			//- Basic information about a function. Used internal.
			//------------------------------------------------------------------------------------------------------------------------
			struct sfunction_data final
			{
				uint64_t m_thread		= 0;
				float m_time			= 0.0f; //- microsecs
				const char* m_name		= nullptr;
				const char* m_category	= nullptr;
				const char* m_file		= nullptr;
				unsigned m_file_line	= 0;
			};

			//- Refined function data gathered and accumulated over time
			//------------------------------------------------------------------------------------------------------------------------
			struct saggregated_function_data final
			{
				sfunction_data m_data;			//- Contains only the latest data about the function
				unsigned m_callcount	= 0;
				float m_time_cumulative = 0.0f;	//- microsecs
				float m_time_peak		= 0.0f;	//- microsecs
				float m_time_average	= 0.0f;	//- microsecs
			};

			//- Scoped object that stores information about current scope and measures time it lived, meaning the time required to
			//- execute that scope.
			//------------------------------------------------------------------------------------------------------------------------
			class cscoped_function final
			{
			public:
				cscoped_function(uint64_t thread, const char* name, const char* category, const char* file, unsigned line);
				~cscoped_function();

			private:
				sfunction_data m_data;
				core::ctimer m_timer;
			};

			//- Basic information about one hardware CPU in the system.
			//------------------------------------------------------------------------------------------------------------------------
			struct scpu_stats final
			{
				string_t m_model_vendor;

				uint8_t m_cores_logical = 0;
				uint8_t m_cores_physical = 0;

				int64_t m_core_max_clock_speed;	//- MHz
				int64_t m_core_clock_speed;		//- MHz
				float m_load = 0.0f;			//- %

				int64_t m_cache_size_L1 = 0;	//- Bytes
				int64_t m_cache_size_L2 = 0;	//- Bytes
				int64_t m_cache_size_L3 = 0;	//- Bytes
			};

			//- Interface for a class to gather stats on cpu and functions and return them back.
			//------------------------------------------------------------------------------------------------------------------------
			class iaggregator
			{
			public:
				virtual ~iaggregator() = default;

				virtual vector_t<scpu_stats>	stats() = 0;
				virtual void					update() = 0;
				virtual void					push(sfunction_data&& data) = 0;

				virtual saggregated_function_data						function_data(stringview_t) = 0;
				virtual umap_t<const char*, saggregated_function_data>	function_data() = 0;
			};

			void set_aggregator(aggregator_ref_t object);
			void set_default_aggregator();
			aggregator_ref_t get_aggregator();

		} //- cpu

		//- Note: do not use directly, use profiler service instead, he will have data if PROFILE_ENABLE was defined and he
		//- also does necessary initialization.
		//------------------------------------------------------------------------------------------------------------------------
		class cprofiler final
		{
		public:
			//- CPU
			static void push(cpu::sfunction_data&& data);
			static vector_t<cpu::scpu_stats> cpu_stats();

			static cpu::saggregated_function_data						function_data(stringview_t name);
			static umap_t<const char*, cpu::saggregated_function_data>	function_data();

			//- Memory
			static memory::smemory_stats memory_stats();
		};

	} //- profile

} //- core