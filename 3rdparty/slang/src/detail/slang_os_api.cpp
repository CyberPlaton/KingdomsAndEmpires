#include "slang_os_api.hpp"

#ifndef SLANG_CUSTOM_ALLOCATOR
	#define USE_DL_PREFIX
	#include "3rdparty/dlmalloc/malloc.h"
#endif

#include <stdarg.h>
#include <cstdio>
#include <cstring>

namespace slang
{
	namespace
	{
		//- @reference: raylib 'TextFormat'
		//------------------------------------------------------------------------------------------------------------------------
		static const char* format(const char* text, ...)
		{
			constexpr auto C_FMT_BUFFERS_MAX = 4;
			constexpr auto C_FMT_BUFFER_LEN_MAX = 1024;

			static char buffers[C_FMT_BUFFERS_MAX][C_FMT_BUFFER_LEN_MAX] = { 0 };
			static int index = 0;

			char* currentBuffer = buffers[index];
			memset(currentBuffer, 0, C_FMT_BUFFER_LEN_MAX);

			va_list args;
			va_start(args, text);
			int requiredByteCount = vsnprintf(currentBuffer, C_FMT_BUFFER_LEN_MAX, text, args);
			va_end(args);

			if (requiredByteCount >= C_FMT_BUFFER_LEN_MAX)
			{
				char* truncBuffer = buffers[index] + C_FMT_BUFFER_LEN_MAX - 4;
				sprintf(truncBuffer, "...");
			}
			if (++index >= C_FMT_BUFFER_LEN_MAX)
			{
				index = 0;
			}
			return currentBuffer;
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void* sos_api::sallocator::malloc(std::size_t s)
	{
		return slang_allocator().m_malloc(s);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void sos_api::sallocator::free(void* p)
	{
		slang_allocator().m_free(p);
	}

	void sos_api::sallocator::free(void* p, std::size_t /*bytes*/)
	{
		slang_allocator().m_free(p);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void* sos_api::sallocator::calloc(std::size_t n, std::size_t s)
	{
		return slang_allocator().m_calloc(n, s);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void* sos_api::sallocator::realloc(void* p, std::size_t s)
	{
		return slang_allocator().m_realloc(p, s);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void* sos_api::sallocator::memalign(std::size_t n, std::size_t s)
	{
		return slang_allocator().m_memalign(n, s);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void* sos_api::sallocator::valloc(std::size_t s)
	{
		return slang_allocator().m_valloc(s);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void slang_init_os_api()
	{
		auto& os_api = sos_api::instance();

		os_api.m_allocator.m_malloc = dlmalloc;
		os_api.m_allocator.m_free = dlfree;
		os_api.m_allocator.m_calloc = dlcalloc;
		os_api.m_allocator.m_realloc = dlrealloc;
		os_api.m_allocator.m_memalign = dlmemalign;
		os_api.m_allocator.m_valloc = dlvalloc;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sos_api::sallocator& slang_allocator()
	{
		return sos_api::instance().m_allocator;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sos_api::slogger& slang_logger()
	{
		return sos_api::instance().m_logger;
	}

} //- slang