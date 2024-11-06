#pragma once
#include "../editor_dock_base.hpp"
#include <tree_sitter.h>

extern "C"
{
	TSLanguage* tree_sitter_cpp();
	TSLanguage* tree_sitter_lua();
}

namespace editor
{
	class ctext_editor;

	namespace detail
	{
		//-------------------------------------------------------------------------------------------------------
		enum language : uint8_t
		{
			language_none = 0,
			language_cpp,
			language_lua,
		};

		//-------------------------------------------------------------------------------------------------------
		enum palette : uint8_t
		{
			palette_none = 0,
			palette_text_keyword,
			palette_text_number,
			palette_text_string,
			palette_text_literal,
			palette_text_punctuation,
			palette_text_preprocessor,
			palette_text_identifier,
			palette_text_resolved_identifier,
			palette_text_comment,
			palette_text_multiline_comment,

			palette_editor_background,
			palette_editor_cursor,
			palette_editor_selection,
			palette_editor_error_marker,
			palette_editor_line_number,
			palette_editor_current_line_fill,
			palette_editor_current_line_fill_inactive,
			palette_editor_current_line_outline,
		};

	} //- detail

	//- Window responsible for editing a source text. By default we target Lua script files to be edited in engine.
	//-------------------------------------------------------------------------------------------------------
	class ctext_editor final : public clayer_base
	{
	public:
		ctext_editor(scontext& ctx) : clayer_base("##text_editor", ctx) {}
		~ctext_editor() = default;

		bool init() override final;
		void shutdown() override final;
		void on_update(float dt) override final;
		void on_ui_render() override final;

	private:
		using text_lines_t = vector_t<string_t>;
		using flags_t = int;

		enum flag : uint8_t
		{
			flag_none	= BIT(0),
			flag_dirty	= BIT(1),
		};

		struct simgui_data final
		{
			vec2_t m_window_position;
			vec2_t m_window_size;
			vec2_t m_mouse_position;
			vec2_t m_cursor_position; //- Position of the ImGui drawing cursor, i.e. screen space position where we will be drawing
		};

		struct sdata final
		{
			text_lines_t m_source_lines;
			simgui_data m_imgui_data;

			float m_zoom	= 1.0f;
			flags_t m_flags = flag_none;
		};

		sdata m_data;
		core::cstring_buffer m_source_buffer;
		string_t m_loaded_text_file;
		string_t m_selected_text_file;
		detail::language m_language = detail::language_lua;

	private:
		void show_main_menu();
		void show_main_window();
		void show_text_editor();

		void load_from_file(const fs::cfileinfo& info);
		void save_to_file(const fs::cfileinfo& info);

		void update_common();
		void update_key_input();
		void update_mouse_input();
		void update_shortcuts();

		ivec2_t mouse_to_line_column() const;
		inline bool dirty() const { return algorithm::bit_check(m_data.m_flags, flag_dirty); }
		inline void set_dirty(const bool value) { value ? algorithm::bit_set(m_data.m_flags, flag_dirty) : algorithm::bit_clear(m_data.m_flags, flag_dirty); }
	};

} //- editor