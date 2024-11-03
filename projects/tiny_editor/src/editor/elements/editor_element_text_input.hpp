#pragma once
#include <core.h>
#include <engine.h>
#include <spritemancer.h>

namespace editor::ui
{
	//- Class responsible for handling text input.
	//------------------------------------------------------------------------------------------------------------------------
	class ctext_input final
	{
	public:
		using options_t = int;
		enum options
		{
			options_none				= 0,
			options_chars_decimal		= ImGuiInputTextFlags_CharsDecimal,
			options_chars_hexa_decimal	= ImGuiInputTextFlags_CharsHexadecimal,
			options_chars_uppercase		= ImGuiInputTextFlags_CharsUppercase,
			options_chars_no_blank		= ImGuiInputTextFlags_CharsNoBlank,
			options_true_on_enter		= ImGuiInputTextFlags_EnterReturnsTrue,
			options_allow_tab			= ImGuiInputTextFlags_AllowTabInput,
			options_readonly			= ImGuiInputTextFlags_ReadOnly,
			options_password			= ImGuiInputTextFlags_Password,
			options_disable_undo_redo	= ImGuiInputTextFlags_NoUndoRedo,
			options_escape_clear_all	= ImGuiInputTextFlags_EscapeClearsAll,
			option_ctrl_enter_new_line	= ImGuiInputTextFlags_CtrlEnterForNewLine,
		};

		static constexpr options_t C_DEFAULT_OPTIONS = options_true_on_enter;

		ctext_input(stringview_t id = {}, bool* enabled = nullptr, options_t flags = C_DEFAULT_OPTIONS);
		~ctext_input() = default;

		ctext_input& value(string_t* text);
		ctext_input& hint(stringview_t text);
		ctext_input& multiline(const bool value);
		ctext_input& resizeable(const bool value);
		ctext_input& size(const vec2_t& value);
		ctext_input& options(int op);
		ctext_input& tooltip(stringview_t text, const bool show = true);

		bool draw();

	private:
		string_t m_id;
		string_t* m_value;
		stringview_t m_tooltip;
		stringview_t m_hint;
		ImVec2 m_size;
		options_t m_flags;
		bool m_show_tooltip;
		bool m_enabled;
		bool m_result = false;

	private:
		bool input_text(stringview_t label, stringview_t hint_text, const ImVec2& size, string_t* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback cb, void* user_data);
	};

} //- editor::ui