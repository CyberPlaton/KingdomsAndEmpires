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
		using flags_t = int;

		enum flag : uint8_t
		{
			flag_none		= BIT(0),
			flag_dirty		= BIT(1),
			flag_readonly	= BIT(2),
		};

		enum language : uint8_t
		{
			language_none = 0,
			language_cpp,
			language_lua,
		};

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

		using text_lines_t = vector_t<string_t>;

		struct ssettings final
		{
			float m_line_spacing = 1.0f;
			float m_tab_size = 4.0f;
		};

		struct simgui_data final
		{
			vec2_t m_window_position;
			vec2_t m_window_size;
			vec2_t m_mouse_position;
			vec2_t m_cursor_position; //- Position of the ImGui drawing cursor, i.e. screen space position where we will be drawing
			bool m_window_focused = false;
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
		language m_language = language_lua;

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
		string_t assemble_title() const;
		inline bool focused() const { return m_data.m_imgui_data.m_window_focused; }
		inline bool readonly() const { return algorithm::bit_check(m_data.m_flags, flag_readonly); }
		inline bool dirty() const { return algorithm::bit_check(m_data.m_flags, flag_dirty); }
		inline void set_dirty(const bool value) { value ? algorithm::bit_set(m_data.m_flags, flag_dirty) : algorithm::bit_clear(m_data.m_flags, flag_dirty); }
	};

} //- editor