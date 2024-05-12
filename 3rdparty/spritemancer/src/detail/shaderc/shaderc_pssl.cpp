/*
 * Copyright 2011-2024 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include "sm_shaderc.hpp"

using namespace sm::shaderc;

namespace bgfx
{

} // namespace bgfx

namespace sm::shaderc
{
	//------------------------------------------------------------------------------------------------------------------------
	bool compilePSSLShader(const Options& _options, uint32_t _version, const std::string& _code,
		bx::WriterI* _shaderWriter, bx::WriterI* _messageWriter)
	{
		BX_UNUSED(_options, _version, _code, _shaderWriter);
		bx::ErrorAssert messageErr;
		bx::write(_messageWriter, &messageErr, "PSSL compiler is not supported.\n");
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const char* getPsslPreamble()
	{
		return {};
	}

} //- sm::shaderc