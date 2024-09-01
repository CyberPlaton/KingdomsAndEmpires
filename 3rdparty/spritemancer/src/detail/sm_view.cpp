#include "sm_view.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	cview::~cview()
	{
		crendertarget::destroy(target());
		cprogram::destroy(program());
	}

} //- sm