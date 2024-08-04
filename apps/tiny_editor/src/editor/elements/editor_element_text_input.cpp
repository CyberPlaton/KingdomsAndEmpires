#include "editor_element_text_input.hpp"

namespace editor::ui
{
	namespace
	{
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
	ctext_input::ctext_input(stringview_t id /*= {}*/, bool* enabled /*= nullptr*/,
		ImGuiInputTextFlags flags /*= ImGuiInputTextFlags_None*/) :
		m_flags(flags), m_id(id.data()), m_enabled(enabled ? *enabled : true), m_value(nullptr), m_size(ImVec2(0, 0))
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctext_input::~ctext_input()
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
		if (value)
		{
			algorithm::bit_set(m_flags, ImGuiInputTextFlags_Multiline);
		}
		else
		{
			algorithm::bit_clear(m_flags, ImGuiInputTextFlags_Multiline);
		}
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctext_input& ctext_input::size(const vec2_t& value)
	{
		m_size = { value.x, value.y };
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctext_input& ctext_input::option(options op, const bool value)
	{
		if (value)
		{
			algorithm::bit_set(m_flags, (ImGuiInputTextFlags)op);
		}
		else
		{
			algorithm::bit_clear(m_flags, (ImGuiInputTextFlags)op);
		}
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

		m_result = input_text(m_id, m_hint, m_size, m_value, m_flags, input_text_callback, nullptr);

		return m_result;
	}

} //- editor::ui