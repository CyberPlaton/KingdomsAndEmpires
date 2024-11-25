#pragma once
#include "detail/core_config.hpp"
#include "detail/core_algorithm.hpp"
#include "detail/core_allocator.hpp"
#include "detail/core_args.hpp"
#include "detail/core_any.hpp"
#include "detail/core_async.hpp"
#include "detail/core_color.hpp"
#include "detail/core_dynamic_pool.hpp"
#include "detail/core_event_service.hpp"
#include "detail/core_filesystem.hpp"
#include "detail/core_future_type.hpp"
#include "detail/core_general_allocator.hpp"
#include "detail/core_input.hpp"
#include "detail/core_io.hpp"
#include "detail/core_linear_allocator.hpp"
#include "detail/core_linked_tree.hpp"
#include "detail/core_logging.hpp"
#include "detail/core_math.hpp"
#include "detail/core_memory.hpp"
#include "detail/core_mutex.hpp"
#include "detail/core_pool_allocator.hpp"
#include "detail/core_platform.hpp"
#include "detail/core_profiler.hpp"
#include "detail/core_random.hpp"
#include "detail/core_rect.hpp"
#include "detail/core_registrator.hpp"
#include "detail/core_resource_manager.hpp"
#include "detail/core_service_manager.hpp"
#include "detail/core_stack_allocator.hpp"
#include "detail/core_string.hpp"
#include "detail/core_stringbuffer.hpp"
#include "detail/core_stringutils.hpp"
#include "detail/core_stringview.hpp"
#include "detail/core_timer.hpp"
#include "detail/core_uuid.hpp"

namespace core
{
	//- Structure to retrieve underlying information about platform, configuration etc.
	//------------------------------------------------------------------------------------------------------------------------
	struct sinfo final
	{
		static string_t platform();
		static string_t architecture();
		static string_t compiler();
		static string_t configuration();
	};

	//- Structure to retrieve central structures and helpers etc.
	//------------------------------------------------------------------------------------------------------------------------
	struct sentry final
	{
		static void init();
		static void shutdown();
		static iallocator* get_allocator();
	};

} //- core
