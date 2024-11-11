#pragma once
#include "../editor_dock_base.hpp"
#include "editor_text_editor_backend.hpp"

namespace editor
{
	namespace textedit
	{
		//-------------------------------------------------------------------------------------------------------
		struct sdata final
		{
			//- General data
			core::srect m_viewport	= { 0.0f, 0.0f, 0.0f, 0.0f };
			vec2_t m_mouse_position	= { 0.0f, 0.0f };
			vec2_t m_cursor_position= { 0.0f, 0.0f }; //- Position of the ImGui drawing cursor, i.e. screen space position where we will be drawing
			float m_zoom			= 1.0f;
			bool m_window_focused	= false;

			//- Render state data
			color_palette m_current_color	= color_palette_none;
			string_t m_current_string;
			float m_current_string_length	= 0.0f;
		};

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
			ctext_editor_backend m_backend;
			sdata m_data;
			sstyle m_style;

			string_t m_loaded_text_file;
			string_t m_selected_text_file;
			language m_language = language_lua;
			text_editor_flags_t m_flags = text_editor_flag_none;

		private:
			void update();
			bool handle_io();
			void show_main_menu();
			void show_main_window();

			void render_tabulator(const sglyph& g);
			void render_whitespace(const sglyph& g);

			void load_from_file(const fs::cfileinfo& info);
			void save_to_file(const fs::cfileinfo& info);

			inline bool focused() const { return m_data.m_window_focused; }
			inline bool readonly() const { return algorithm::bit_check(m_flags, text_editor_flag_readonly); }
			inline bool dirty() const { return algorithm::bit_check(m_flags, text_editor_flag_dirty); }
			inline void set_dirty(const bool value) { value ? algorithm::bit_set(m_flags, text_editor_flag_dirty) : algorithm::bit_clear(m_flags, text_editor_flag_dirty); }
		};

	} //- textedit

} //- editor