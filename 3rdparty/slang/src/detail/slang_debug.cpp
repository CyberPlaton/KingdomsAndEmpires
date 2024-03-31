#include "slang_debug.hpp"
#include <stdarg.h>
#include <cstdio>
#include <cstring>

namespace slang
{
	namespace
	{
		//- @reference: raylib 'TextFormat'
		//------------------------------------------------------------------------------------------------------------------------
		static const char* format(const char* text, ...)
		{
			constexpr auto C_FMT_BUFFERS_MAX = 4;
			constexpr auto C_FMT_BUFFER_LEN_MAX = 1024;

			static char buffers[C_FMT_BUFFERS_MAX][C_FMT_BUFFER_LEN_MAX] = { 0 };
			static int index = 0;

			char* currentBuffer = buffers[index];
			memset(currentBuffer, 0, C_FMT_BUFFER_LEN_MAX);

			va_list args;
			va_start(args, text);
			int requiredByteCount = vsnprintf(currentBuffer, C_FMT_BUFFER_LEN_MAX, text, args);
			va_end(args);

			if (requiredByteCount >= C_FMT_BUFFER_LEN_MAX)
			{
				char* truncBuffer = buffers[index] + C_FMT_BUFFER_LEN_MAX - 4;
				sprintf(truncBuffer, "...");
			}
			if (++index >= C_FMT_BUFFER_LEN_MAX)
			{
				index = 0;
			}
			return currentBuffer;
		}

	} //- unnamed


} //- slang