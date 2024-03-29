#pragma once
#include "slang_config.hpp"

namespace slang
{
	typedef
	void*(*slang_malloc_t)(std::size_t);

	typedef
	void(*slang_free_t)(void*);

	typedef
	void*(*slang_calloc_t)(std::size_t, std::size_t);

	typedef
	void*(*slang_realloc_t)(void*, std::size_t);

	typedef
	void*(*slang_memalign_t)(std::size_t, std::size_t);

	typedef
	void*(*slang_valloc_t)(std::size_t);

	typedef
	void(*slang_logger_t)(uint8_t, const char*);

	//- Customizable minimal OS interface
	//------------------------------------------------------------------------------------------------------------------------
	struct sos_api
	{
		static inline sos_api& instance() { static sos_api s_os_api; return s_os_api; }

		struct sallocator
		{
			slang_malloc_t m_malloc = nullptr;
			slang_free_t m_free = nullptr;
			slang_calloc_t m_calloc = nullptr;
			slang_realloc_t m_realloc = nullptr;
			slang_memalign_t m_memalign = nullptr;
			slang_valloc_t m_valloc = nullptr;
		};

		struct slogger
		{
			enum log_level : int8_t
			{
				log_level_none = -1,
				log_level_trace = 0,
				log_level_debug,
				log_level_info,
				log_level_warn,
				log_level_error,
				log_level_critical,
			};

			slang_logger_t m_log = nullptr;
		};

		sallocator m_allocator;
		slogger m_logger;
	};

	static void slang_init_os_api();
	static sos_api::sallocator& slang_allocator();
	static sos_api::slogger& slang_logger();

} //- slang