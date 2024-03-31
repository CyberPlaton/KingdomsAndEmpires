#include "slang_os_api.hpp"

#ifndef SLANG_CUSTOM_ALLOCATOR
	#define USE_DL_PREFIX
	#include "3rdparty/dlmalloc/malloc.h"
#endif

namespace slang
{
	namespace
	{

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void* detail::sallocator::malloc(std::size_t s)
	{
		return slang_allocator().m_malloc(s);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void detail::sallocator::free(void* p)
	{
		slang_allocator().m_free(p);
	}

	void detail::sallocator::free(void* p, std::size_t /*bytes*/)
	{
		slang_allocator().m_free(p);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void* detail::sallocator::calloc(std::size_t n, std::size_t s)
	{
		return slang_allocator().m_calloc(n, s);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void* detail::sallocator::realloc(void* p, std::size_t s)
	{
		return slang_allocator().m_realloc(p, s);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void* detail::sallocator::memalign(std::size_t n, std::size_t s)
	{
		return slang_allocator().m_memalign(n, s);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void* detail::sallocator::valloc(std::size_t s)
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
	detail::sallocator& slang_allocator()
	{
		return sos_api::instance().m_allocator;
	}

	//------------------------------------------------------------------------------------------------------------------------
	detail::slogger& slang_logger()
	{
		return sos_api::instance().m_logger;
	}

} //- slang