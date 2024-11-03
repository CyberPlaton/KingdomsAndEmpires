#include "editor_element_text_input.hpp"

namespace editor::ui
{
	namespace
	{
		constexpr uint64_t C_STRING_TOOLTIP_MINIMAL_SIZE = 25;
		constexpr uint64_t C_STRING_CAPACITY_DEFAULT = 64;
		constexpr stringview_t C_INPUT_TEXT_ID = "##inputTextId";

		//------------------------------------------------------------------------------------------------------------------------
		struct sinput_text_callback_user_data
		{
			string_t* m_string = nullptr;
			ImGuiInputTextCallback  m_callback = nullptr;
			void* m_user_data = nullptr;
		};

		//------------------------------------------------------------------------------------------------------------------------
		void input_text_resize_string(string_t* string, int length)
		{
			string->resize(SCAST(uint64_t, length));
		}

		//------------------------------------------------------------------------------------------------------------------------
		int input_text_callback(ImGuiInputTextCallbackData* data)
		{
			auto* user_data = (sinput_text_callback_user_data*)data->UserData;
			if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
			{
				//- Resize string callback. If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
				auto* string = user_data->m_string;

				CORE_ASSERT(data->Buf == string->c_str(), "Invalid operation");

				input_text_resize_string(string, data->BufTextLen);

				data->Buf = (char*)string->c_str();
			}
			else if (user_data->m_callback)
			{
				//- Forward to user callback, if any
				data->UserData = user_data->m_user_data;
				return user_data->m_callback(data);
			}
			return 0;
		}

		//- A good candidate for base class functionality for all elements, we use flags all over the place and dont want to
		//- solve the same problem everywhere each time anew...
		//------------------------------------------------------------------------------------------------------------------------
		template<typename TBit>
		void set_flag(int& byte, TBit bit, const bool value)
		{
			if(value) algorithm::bit_set(byte, bit);
			else algorithm::bit_clear(byte, bit);
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool ctext_input::input_text(stringview_t label, stringview_t hint_text, const ImVec2& size, string_t* str,
		ImGuiInputTextFlags flags, ImGuiInputTextCallback cb, void* user_data)
	{
		sinput_text_callback_user_data data;
		data.m_string = str;
		data.m_callback = cb;
		data.m_user_data = user_data;

		return ImGui::InputTextEx(label.data(), hint_text.data(), (char*)str->c_str(), str->capacity() + 1, size, flags, input_text_callback, &data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctext_input::ctext_input(stringview_t id /*= {}*/, bool* enabled /*= nullptr*/, options_t flags /*= C_DEFAULT_OPTIONS*/) :
		m_flags(flags), m_id(id.data()), m_enabled(enabled ? *enabled : true), m_value(nullptr), m_size(ImVec2(0, 0)), m_result(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctext_input& ctext_input::value(string_t* text)
	{
		CORE_ASSERT(text, "Invalid operation. String to be edited is 'nullptr'!");

		m_value = text;

		if (m_value->capacity() == 0) { m_value->reserve(C_STRING_CAPACITY_DEFAULT); }

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctext_input& ctext_input::hint(stringview_t text)
	{
		m_hint = text;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctext_input& ctext_input::multiline(const bool value)
	{
		set_flag(m_flags, ImGuiInputTextFlags_Multiline, value);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctext_input& ctext_input::resizeable(const bool value)
	{
		set_flag(m_flags, ImGuiInputTextFlags_CallbackResize, value);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctext_input& ctext_input::size(const vec2_t& value)
	{
		m_size = { value.x, value.y };
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctext_input& ctext_input::options(int op)
	{
		m_flags |= (ImGuiInputTextFlags_)op;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctext_input& ctext_input::tooltip(stringview_t text, const bool show /*= true*/)
	{
		m_tooltip = text;

		//- FIXME: reconsider how to handle options for when to show tooltips...
		//- Here we take away the minimal length option as a temporary solution.
		m_show_tooltip = show && strlen(text.data()) > C_STRING_TOOLTIP_MINIMAL_SIZE;

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool ctext_input::draw()
	{
		if (m_id.empty())
		{
			m_id = fmt::format("{} {}", C_INPUT_TEXT_ID.data(), m_hint.data());
		}

		CORE_ASSERT(!m_id.empty(), "Invalid operation. Identifier cannot be empty, defined a valid Id or a unique Hint!");

		{
			imgui::cdisabled_scope disabled_scope(!m_enabled);

			m_result = input_text(m_id, m_hint, m_size, m_value, SCAST(ImGuiInputTextFlags, m_flags), input_text_callback, nullptr);
		}

		if (m_show_tooltip)
		{
			imgui::cui::help_marker_no_question_mark(m_tooltip.data(), imgui::tooltip_hovering_delay_normal);
		}

		return m_result;
	}

} //- editor::ui