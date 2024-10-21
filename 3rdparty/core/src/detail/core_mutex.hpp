#pragma once
#include "core_config.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	class cmutex final
	{
	public:
		cmutex();
		~cmutex();

		void lock();
		void unlock();

	private:
#if CORE_PLATFORM_WINDOWS
		__declspec(align(16)) uint8_t m_internal[64];
#else
		__attribute__((aligned(16))) uint8_t m_internal[64];
#endif
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cscope_mutex final
	{
	public:
		cscope_mutex(cmutex& m);
		~cscope_mutex();
		cscope_mutex& operator=(const cscope_mutex&) = delete;
		cscope_mutex(const cscope_mutex&) = delete;

	private:
		cmutex& m_mutex;
	};

} //- core