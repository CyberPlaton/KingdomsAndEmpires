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
			return SLANG_MALLOC(s);
		}

		void sallocator::free(void* p, std::size_t /*bytes*/)
		{
			SLANG_FREE(p);
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