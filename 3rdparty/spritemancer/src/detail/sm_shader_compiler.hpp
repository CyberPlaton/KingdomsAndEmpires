#pragma once
#include "sm_config.hpp"

namespace sm::shaderc
{
	memory_ref_t compile(stringview_t code, const soptions& options);

} //- sm::shaderc