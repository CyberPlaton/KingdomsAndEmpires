#include "core_profiler.hpp"
#include "core_mutex.hpp"

namespace core
{
	namespace profile
	{
		namespace memory
		{
			namespace
			{
				constexpr stringview_t C_TOTAL_RESERVED_MEMORY_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_PEAK_TOTAL_RESERVED_MEMORY_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_TOTAL_USED_MEMORY_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_PEAK_TOTAL_USED_MEMORY_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_ELAPSED_PROCESS_TIME_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_ELAPSED_USER_CODE_TIME_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_ELAPSED_SYSTEM_CODE_TIME_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_TOTAL_RESIDENT_SET_SIZE_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_PEAK_TOTAL_RESIDENT_SET_SIZE_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_NORMAL_ALLOCATIONS_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_BIG_ALLOCATIONS_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_HUGE_ALLOCATIONS_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_MODIFICATIONS_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_RESET_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_PURGE_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_SYSTEM_CALL_MAP_MEMORY_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_SYSTEM_CALL_COMMIT_MEMORY_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_FREE_BLOCK_SEARCH_OPS_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_SEGMENTS_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_PAGES_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				static core::cmutex S_MUTEX;
				static aggregator_ref_t S_AGGREGATOR = nullptr;

				//------------------------------------------------------------------------------------------------------------------------
				static mi_stats_t* mi_stats_get_default(void)
				{
					mi_heap_t* heap = mi_heap_get_default();
					return &heap->tld->stats;
				}

				//------------------------------------------------------------------------------------------------------------------------
				template<typename TValue>
				static smemory_stats_entry pair(TValue value, stringview_t desc)
				{
					return { static_cast<uint64_t>(value), desc };
				}

				//- Note: be careful when watching mimalloc stats, it seems that each thread has its own stats making the data
				//- not that reliable.
				//------------------------------------------------------------------------------------------------------------------------
				class cdefault_aggregator final : public iaggregator
				{
				public:
					cdefault_aggregator() = default;
					~cdefault_aggregator() = default;

					smemory_stats	stats() override final;
					void			update() override final;

				private:
					smemory_stats m_current;
				};

				//------------------------------------------------------------------------------------------------------------------------
				void cdefault_aggregator::update()
				{
					core::cscope_mutex m(S_MUTEX);

					//- Gather and update current data
					uint64_t elapsed;
					uint64_t user_time;
					uint64_t sys_time;
					uint64_t current_rss;
					uint64_t peak_rss;
					uint64_t current_commit;
					uint64_t peak_commit;
					uint64_t page_faults;
					mi_process_info(&elapsed, &user_time, &sys_time, &current_rss, &peak_rss, &current_commit, &peak_commit, &page_faults);

					const auto* mistats = mi_stats_get_default();

					m_current.m_total_reserved_memory = pair(mistats->reserved.current, C_TOTAL_RESERVED_MEMORY_DESC);
					m_current.m_peak_total_reserved_memory = pair(mistats->reserved.peak, C_PEAK_TOTAL_RESERVED_MEMORY_DESC);
					m_current.m_total_used_memory = pair(mistats->committed.current, C_TOTAL_USED_MEMORY_DESC);
					m_current.m_peak_total_used_memory = pair(mistats->committed.peak, C_PEAK_TOTAL_USED_MEMORY_DESC);

					m_current.m_elapsed_process_time = pair(elapsed, C_ELAPSED_PROCESS_TIME_DESC);
					m_current.m_elapsed_user_code_time = pair(user_time, C_ELAPSED_USER_CODE_TIME_DESC);
					m_current.m_elapsed_system_code_time = pair(sys_time, C_ELAPSED_SYSTEM_CODE_TIME_DESC);

					m_current.m_total_resident_set_size = pair(current_rss, C_TOTAL_RESIDENT_SET_SIZE_DESC);
					m_current.m_peak_total_resident_set_size = pair(peak_rss, C_PEAK_TOTAL_RESIDENT_SET_SIZE_DESC);

					m_current.m_normal_allocations_bytes = pair(mistats->normal.current, C_NORMAL_ALLOCATIONS_COUNT_DESC);
					m_current.m_large_allocations_bytes = pair(mistats->large.current, C_BIG_ALLOCATIONS_COUNT_DESC);
					m_current.m_huge_allocations_bytes = pair(mistats->huge.current, C_HUGE_ALLOCATIONS_COUNT_DESC);

					//- Note: for mi_stat_counter_t we only care for 'total' value, i.e. the accumulated bytes or operation counts,
					//- the frequency of those operations occuring is denoted with 'count'.
					m_current.m_modifications_count = pair(mistats->page_committed.current, C_MODIFICATIONS_COUNT_DESC);
					m_current.m_reset_count = pair(mistats->reset_calls.total, C_RESET_COUNT_DESC);
					m_current.m_purge_count = pair(mistats->purge_calls.total, C_PURGE_COUNT_DESC);
					m_current.m_system_call_memory_map_count = pair(mistats->mmap_calls.total, C_SYSTEM_CALL_MAP_MEMORY_DESC);
					m_current.m_system_call_memory_commit_count = pair(mistats->commit_calls.total, C_SYSTEM_CALL_COMMIT_MEMORY_DESC);
					m_current.m_free_block_search_operations_count = pair(mistats->searches.total, C_FREE_BLOCK_SEARCH_OPS_COUNT_DESC);

					m_current.m_segments_count = pair(mistats->segments.current, C_SEGMENTS_COUNT_DESC);
					m_current.m_pages_count = pair(mistats->pages.current, C_PAGES_COUNT_DESC);
				}

				//------------------------------------------------------------------------------------------------------------------------
				smemory_stats cdefault_aggregator::stats()
				{
					core::cscope_mutex m(S_MUTEX);

					return m_current;
				}

			} //- unnamed

			//------------------------------------------------------------------------------------------------------------------------
			void set_aggregator(aggregator_ref_t object)
			{
				S_AGGREGATOR = object;
			}

			//------------------------------------------------------------------------------------------------------------------------
			aggregator_ref_t get_aggregator()
			{
				return S_AGGREGATOR;
			}

			//------------------------------------------------------------------------------------------------------------------------
			void set_default_aggregator()
			{
#if MIMALLOC_ENABLE
				S_AGGREGATOR = std::make_shared<cdefault_aggregator>();

				//- Gather initial data
				S_AGGREGATOR->update();
#endif
			}

		} //- memory

		namespace cpu
		{
			namespace
			{
				static core::cmutex S_MUTEX;
				static aggregator_ref_t S_AGGREGATOR = nullptr;

				//- Class responsible for aggregating raw CPU and function profiling data.
				//------------------------------------------------------------------------------------------------------------------------
				class cdefault_aggregator final : public iaggregator
				{
				public:
					cdefault_aggregator() = default;
					~cdefault_aggregator() = default;

					vector_t<scpu_stats>	stats() override final;
					void					update() override final;
					void					push(sfunction_data&& data) override final;

					saggregated_function_data						function_data(stringview_t name) override final;
					umap_t<const char*, saggregated_function_data>	function_data() override final;

				private:
					umap_t<const char*, saggregated_function_data> m_functions;
					stack_t<sfunction_data> m_stack;
					vector_t<scpu_stats> m_current;

				private:
					void gather_hardware_information();
					void gather_function_information();
				};

				//------------------------------------------------------------------------------------------------------------------------
				vector_t<cpu::scpu_stats> cdefault_aggregator::stats()
				{
					core::cscope_mutex m(S_MUTEX);

					return m_current;
				}

				//------------------------------------------------------------------------------------------------------------------------
				void cdefault_aggregator::update()
				{
					core::cscope_mutex m(S_MUTEX);

					gather_hardware_information();
					gather_function_information();
				}

				//------------------------------------------------------------------------------------------------------------------------
				void cdefault_aggregator::push(sfunction_data&& data)
				{
					core::cscope_mutex m(S_MUTEX);

					m_stack.push(std::move(data));
				}

				//------------------------------------------------------------------------------------------------------------------------
				saggregated_function_data cdefault_aggregator::function_data(stringview_t name)
				{
					core::cscope_mutex m(S_MUTEX);

					return m_functions.at(name.data());
				}

				//------------------------------------------------------------------------------------------------------------------------
				umap_t<const char*, saggregated_function_data> cdefault_aggregator::function_data()
				{
					core::cscope_mutex m(S_MUTEX);

					return m_functions;
				}

				//------------------------------------------------------------------------------------------------------------------------
				void cdefault_aggregator::gather_hardware_information()
				{
					m_current.clear();

					for (const auto& info : hwinfo::getAllCPUs())
					{
						scpu_stats cpu_stat;

						cpu_stat.m_model_vendor = fmt::format("{}-{}", info.vendor(), info.modelName());
						cpu_stat.m_cores_logical = static_cast<uint8_t>(info.numLogicalCores());
						cpu_stat.m_cores_physical = static_cast<uint8_t>(info.numPhysicalCores());
						cpu_stat.m_core_max_clock_speed = info.maxClockSpeed_MHz();
						cpu_stat.m_core_clock_speed = info.regularClockSpeed_MHz();
						cpu_stat.m_load = static_cast<float>(info.currentUtilisation());
						cpu_stat.m_cache_size_L1 = info.L1CacheSize_Bytes();
						cpu_stat.m_cache_size_L2 = info.L2CacheSize_Bytes();
						cpu_stat.m_cache_size_L3 = info.L3CacheSize_Bytes();

						m_current.push_back(cpu_stat);
					}
				}

				//------------------------------------------------------------------------------------------------------------------------
				void cdefault_aggregator::gather_function_information()
				{
					while (!m_stack.empty())
					{
						const auto& fun = m_stack.top(); m_stack.pop();

						auto& data = m_functions[fun.m_name];

						data.m_data.m_thread = fun.m_thread;
						data.m_data.m_time = fun.m_time;
						data.m_data.m_name = fun.m_name;
						data.m_data.m_category = fun.m_category;
						data.m_data.m_file = fun.m_file;
						data.m_data.m_file_line = fun.m_file_line;

						++data.m_callcount;

						data.m_time_cumulative += fun.m_time;
						data.m_time_peak = glm::max(data.m_time_peak, fun.m_time);
						data.m_time_average = (data.m_time_cumulative / static_cast<float>(data.m_callcount));
					}
				}

			} //- unnamed

			//------------------------------------------------------------------------------------------------------------------------
			void set_aggregator(aggregator_ref_t object)
			{
				S_AGGREGATOR = object;
			}

			//------------------------------------------------------------------------------------------------------------------------
			void set_default_aggregator()
			{
				S_AGGREGATOR = std::make_shared<cdefault_aggregator>();
			}

			//------------------------------------------------------------------------------------------------------------------------
			aggregator_ref_t get_aggregator()
			{
				return S_AGGREGATOR;
			}

			//------------------------------------------------------------------------------------------------------------------------
			cscoped_function::cscoped_function(uint64_t thread, const char* name, const char* category, const char* file, unsigned line) :
				m_data({ thread, 0.0f, name, category, file, line })
			{
				m_timer.start();
			}

			//------------------------------------------------------------------------------------------------------------------------
			cscoped_function::~cscoped_function()
			{
				m_data.m_time = m_timer.microsecs();

				core::profile::cprofiler::push(std::move(m_data));
			}

		} //- cpu

		//------------------------------------------------------------------------------------------------------------------------
		void cprofiler::push(cpu::sfunction_data&& data)
		{
			CORE_ASSERT(cpu::get_aggregator(), "Invalid operation. No CPU aggregator was set!");

			cpu::get_aggregator()->push(std::move(data));
		}

		//------------------------------------------------------------------------------------------------------------------------
		vector_t<cpu::scpu_stats> cprofiler::cpu_stats()
		{
			CORE_ASSERT(cpu::get_aggregator(), "Invalid operation. No CPU aggregator was set!");

			return cpu::get_aggregator()->stats();
		}

		//------------------------------------------------------------------------------------------------------------------------
		memory::smemory_stats cprofiler::memory_stats()
		{
			CORE_ASSERT(memory::get_aggregator(), "Invalid operation. No Memory aggregator was set!");

			return memory::get_aggregator()->stats();
		}

		//------------------------------------------------------------------------------------------------------------------------
		cpu::saggregated_function_data cprofiler::function_data(stringview_t name)
		{
			CORE_ASSERT(cpu::get_aggregator(), "Invalid operation. No CPU aggregator was set!");

			return cpu::get_aggregator()->function_data(name);
		}

		//------------------------------------------------------------------------------------------------------------------------
		umap_t<const char*, cpu::saggregated_function_data> cprofiler::function_data()
		{
			CORE_ASSERT(cpu::get_aggregator(), "Invalid operation. No CPU aggregator was set!");

			return cpu::get_aggregator()->function_data();
		}

	} //- profile

} //- core