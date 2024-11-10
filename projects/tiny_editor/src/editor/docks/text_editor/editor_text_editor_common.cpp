#include "editor_text_editor_common.hpp"

namespace editor
{
	namespace textedit
	{
		//-------------------------------------------------------------------------------------------------------
		unsigned utf8_char_length(const char& c)
		{
			if ((c & 0xFE) == 0xFC) return 6;
			if ((c & 0xFC) == 0xF8) return 5;
			if ((c & 0xF8) == 0xF0) return 4;
			if ((c & 0xF0) == 0xE0) return 3;
			if ((c & 0xE0) == 0xC0) return 2;
			return 1;
		}

		//-------------------------------------------------------------------------------------------------------
		unsigned char_to_utf8(char* buffer, unsigned buffer_length, char_t c)
		{
			if (c < 0x80)
			{
				buffer[0] = (char)c;
				return 1;
			}
			if (c < 0x800)
			{
				if (buffer_length < 2) return 0;
				buffer[0] = (char)(0xc0 + (c >> 6));
				buffer[1] = (char)(0x80 + (c & 0x3f));
				return 2;
			}
			if (c >= 0xdc00 && c < 0xe000)
			{
				return 0;
			}
			if (c >= 0xd800 && c < 0xdc00)
			{
				if (buffer_length < 4) return 0;
				buffer[0] = (char)(0xf0 + (c >> 18));
				buffer[1] = (char)(0x80 + ((c >> 12) & 0x3f));
				buffer[2] = (char)(0x80 + ((c >> 6) & 0x3f));
				buffer[3] = (char)(0x80 + ((c) & 0x3f));
				return 4;
			}
			//else if (c < 0x10000)
			{
				if (buffer_length < 3) return 0;
				buffer[0] = (char)(0xe0 + (c >> 12));
				buffer[1] = (char)(0x80 + ((c >> 6) & 0x3f));
				buffer[2] = (char)(0x80 + ((c) & 0x3f));
				return 3;
			}
		}

	}//- textedit

} //- editor