#include "slang_config.hpp"

namespace slang
{
	//------------------------------------------------------------------------------------------------------------------------
	void* callocator::malloc(size_t n)
	{
		return SLANG_MALLOC(n);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void callocator::free(void* p, size_t n)
	{
		SLANG_FREE(p);
	}

} //- slang