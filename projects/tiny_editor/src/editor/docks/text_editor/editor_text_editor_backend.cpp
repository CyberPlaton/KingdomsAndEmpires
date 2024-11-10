#include "editor_text_editor_backend.hpp"

namespace editor
{
	namespace textedit
	{
		//-------------------------------------------------------------------------------------------------------
		bool ctext_editor_backend::init()
		{
			return true;
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor_backend::shutdown()
		{

		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor_backend::update()
		{

		}

		//-------------------------------------------------------------------------------------------------------
		unsigned ctext_editor_backend::line_count() const
		{
			return SCAST(unsigned, m_editing_lines.size());
		}

		//-------------------------------------------------------------------------------------------------------
		const text_line_t& ctext_editor_backend::line_at(unsigned line) const
		{
			return m_editing_lines.at(line);
		}

		//-------------------------------------------------------------------------------------------------------
		const core::cstring_buffer& ctext_editor_backend::source() const
		{
// 			if (dirty() && !readonly())
// 			{
// 				sync_up();
// 			}
			return m_source_buffer;
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor_backend::set_source(const char* text)
		{
			m_source_buffer.write(text);

			sync_down();
		}

		//- Responsible for synchronizing source buffer with edited text lines, basically copy back what was
		//- written in editor to source buffer.
		//-------------------------------------------------------------------------------------------------------
		void ctext_editor_backend::sync_up()
		{
			m_source_buffer.clear();
			string_t s; s.reserve(256);

			for (const auto& line : m_editing_lines)
			{
				for (const auto& glyph : line)
				{
					s.push_back(glyph.m_char);
				}

				m_source_buffer.write(fmt::format("{}\n", s));
				s.clear();
			}
		}

		//- Responsible for synchronizing source buffer with edited text lines, while recreating the edited text lines
		//-------------------------------------------------------------------------------------------------------
		void ctext_editor_backend::sync_down()
		{
			std::stringstream ss(m_source_buffer.c_str());
			string_t s;
			char buffer[7] = {0};

			while (std::getline(ss, s, '\n'))
			{
				auto& line = m_editing_lines.emplace_back();

				for (const auto& c : s)
				{
					auto& glyph = line.emplace_back();
					glyph.m_char = c;
					glyph.m_color = color_palette_none;
					glyph.m_utf8_char_length = utf8_char_length(c);
					glyph.m_utf8_char = c;
				}
			}
		}

	} //- textedit

} //- editor