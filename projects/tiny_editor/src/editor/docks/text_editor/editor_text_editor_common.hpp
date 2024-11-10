#pragma once
#include <core.h>
#include <spritemancer.h>

namespace editor
{
	namespace textedit
	{
		using char_t = ImWchar;
		using text_editor_flags_t = int;

		//-------------------------------------------------------------------------------------------------------
		enum text_editor_flag : uint8_t
		{
			text_editor_flag_none		= BIT(0),
			text_editor_flag_dirty		= BIT(1),
			text_editor_flag_readonly	= BIT(2),
		};

		//-------------------------------------------------------------------------------------------------------
		enum language : uint8_t
		{
			language_none = 0,
			language_cpp,
			language_lua,
		};

		//-------------------------------------------------------------------------------------------------------
		enum color_palette : uint8_t
		{
			color_palette_none = 0,
			color_palette_text_keyword,
			color_palette_text_number,
			color_palette_text_string,
			color_palette_text_literal,
			color_palette_text_punctuation,
			color_palette_text_preprocessor,
			color_palette_text_identifier,
			color_palette_text_resolved_identifier,
			color_palette_text_comment,
			color_palette_text_multiline_comment,

			color_palette_editor_background,
			color_palette_editor_cursor,
			color_palette_editor_selection,
			color_palette_editor_error_marker,
			color_palette_editor_line_number,
			color_palette_editor_current_line_fill,
			color_palette_editor_current_line_fill_inactive,
			color_palette_editor_current_line_outline,
		};

		//- c must be a standalone character (<128) or a leading byte of a UTF-8 encoded code sequence
		unsigned utf8_char_length(const char& c);

		//- 
		unsigned char_to_utf8(char* buffer, unsigned buffer_length, char_t c);

	}//- textedit

} //- editor