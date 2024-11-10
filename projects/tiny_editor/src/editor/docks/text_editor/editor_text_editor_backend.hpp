#pragma once
#include "../../context/editor_context.hpp"
#include "editor_text_editor_common.hpp"
#include <tree_sitter.h>

extern "C"
{
	TSLanguage* tree_sitter_cpp();
	TSLanguage* tree_sitter_lua();
}

namespace editor
{
	namespace textedit
	{
		//- Representing a single character in source file/line.
		//-------------------------------------------------------------------------------------------------------
		struct sglyph final
		{
			char m_char					= '\0';
			unsigned m_utf8_char_length = 0;
			char_t m_utf8_char			= 0;
			color_palette m_color		= color_palette_none;
		};

		using text_line_t = vector_t<sglyph>;
		using text_lines_t = vector_t<text_line_t>;

		//- Responsible for containing the actual textual source buffers, interacting with
		//- tree-sitter, gathers information on symbols and functions etc. and errors.
		//-------------------------------------------------------------------------------------------------------
		class ctext_editor_backend final
		{
		public:
			ctext_editor_backend() = default;
			~ctext_editor_backend() = default;

			bool init();
			void shutdown();
			void update();

			unsigned line_count() const;
			const text_line_t& line_at(unsigned line) const;

			const core::cstring_buffer& source() const;
			void set_source(const char* text);

		private:
			core::cstring_buffer m_source_buffer;	//- Source text buffer before editing
			text_lines_t m_editing_lines;			//- Edited state of text buffer
			text_editor_flags_t m_flags = text_editor_flag_none;

		private:
			void sync_up();
			void sync_down();
			inline bool readonly() const { return algorithm::bit_check(m_flags, text_editor_flag_readonly); }
			inline bool dirty() const { return algorithm::bit_check(m_flags, text_editor_flag_dirty); }
			inline void set_dirty(const bool value) { value ? algorithm::bit_set(m_flags, text_editor_flag_dirty) : algorithm::bit_clear(m_flags, text_editor_flag_dirty); }
		};

	} //- textedit

} //- editor
