#include "core_mutex.hpp"

namespace core
{
	namespace
	{

#if CORE_PLATFORM_WINDOWS || CORE_PLATFORM_XBOXONE || CORE_PLATFORM_XBOXSERIES
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <errno.h>
		typedef CRITICAL_SECTION pthread_mutex_t;
		typedef unsigned pthread_mutexattr_t;
		//------------------------------------------------------------------------------------------------------------------------
		inline int pthread_mutex_lock(pthread_mutex_t* _mutex)
		{
			EnterCriticalSection(_mutex);
			return 0;
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline int pthread_mutex_unlock(pthread_mutex_t* _mutex)
		{
			LeaveCriticalSection(_mutex);
			return 0;
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline int pthread_mutex_trylock(pthread_mutex_t* _mutex)
		{
			return TryEnterCriticalSection(_mutex) ? 0 : EBUSY;
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline int pthread_mutex_init(pthread_mutex_t* _mutex, pthread_mutexattr_t* /*_attr*/)
		{
			InitializeCriticalSection(_mutex);
			return 0;
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline int pthread_mutex_destroy(pthread_mutex_t* _mutex)
		{
			DeleteCriticalSection(_mutex);
			return 0;
		}
#endif

	} //-unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cmutex::cmutex()
	{
		static_assert(sizeof(pthread_mutex_t) <= sizeof(m_internal), "Critical");

		pthread_mutexattr_t attr;

#if CORE_PLATFORM_WINDOWS || CORE_PLATFORM_XBOXONE || CORE_PLATFORM_XBOXSERIES
#else
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#endif
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_init(handle, &attr);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmutex::~cmutex()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_destroy(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmutex::lock()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_lock(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmutex::unlock()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_unlock(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cscope_mutex::cscope_mutex(cmutex& m) :
		m_mutex(m)
	{
		m_mutex.lock();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cscope_mutex::~cscope_mutex()
	{
		m_mutex.unlock();
	}

} //- core