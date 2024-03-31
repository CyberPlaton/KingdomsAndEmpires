#include "slang_os_api.hpp"

namespace slang
{
	namespace
	{

	} //- unnamed

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		void* sallocator::malloc(std::size_t s)
		{
			return slang_allocator().m_malloc(s);
		}

		void sallocator::free(void* p, std::size_t /*bytes*/)
		{
			slang_allocator().m_free(p);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void* sallocator::calloc(std::size_t n, std::size_t s)
		{
			return slang_allocator().m_calloc(n, s);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void* sallocator::realloc(void* p, std::size_t s)
		{
			return slang_allocator().m_realloc(p, s);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void* sallocator::memalign(std::size_t n, std::size_t s)
		{
			return slang_allocator().m_memalign(n, s);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void* sallocator::valloc(std::size_t s)
		{
			return slang_allocator().m_valloc(s);
		}

	} //- detail

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