#include "slang_config.hpp"

namespace slang
{
	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		void sallocator::init()
		{
			m_malloc = dlmalloc;
			m_free = dlfree;
			m_calloc = dlcalloc;
			m_realloc = dlrealloc;
			m_memalign = dlmemalign;
			m_valloc = dlvalloc;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void slogger::init(slang_logger_t callback, log_level level /*= log_level_warn*/)
		{
			if (level > log_level_none)
			{
				m_log = std::move(callback);
				m_level = level;
			}
		}

	} //- detail

} //- slang