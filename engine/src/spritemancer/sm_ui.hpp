#pragma once
#include "sm_common.hpp"
#include "imgui_integration/imgui_integration.hpp"

namespace ImNodes = ax::NodeEditor;
namespace ImGui
{
	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col);
	bool Spinner(const char* label, float radius, int thickness, const ImU32& color);
}

namespace sm
{
	ImNodes::EditorContext* nodeeditor_ctx();

	//------------------------------------------------------------------------------------------------------------------------
	enum notification_type
	{
		notification_type_none = 0,
		notification_type_success,
		notification_type_info,
		notification_type_warn,
		notification_type_error,
		notification_type_critical
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum tooltip_hovering_delay
	{
		tooltip_hovering_delay_none = 0,
		tooltip_hovering_delay_short,
		tooltip_hovering_delay_normal,
		tooltip_hovering_delay_long
	};

	struct snotification;
	class cfile_dialogue;

	//------------------------------------------------------------------------------------------------------------------------
	class cui
	{
	public:
		static bool init();
		static void shutdown();
		static void on_resize(unsigned w, unsigned h);
		static void begin();
		static void end();
		static ImU32 get_imgui_color(float r, float g, float b, float a);
		static ImU32 get_imgui_color(const core::scolor& color);
		static void create_notification(const string_t& label, const string_t& message, notification_type type, unsigned lifetime_seconds = 0, const vec2_t& size = { 400.0f, 100.0f });
		static void create_file_dialog(const string_t& field_text, const string_t& field_desc, bool* is_open, const vec2_t& position, const vec2_t& size, const char* allowed_file_types[], unsigned file_type_count, const string_t& directory, const string_t& ok_text = "OK", const string_t& cancel_text = "Cancel", ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);
		static void create_folder_dialog(const string_t& field_text, const string_t& field_desc, bool* is_open, const vec2_t& position, const vec2_t& size, const string_t& directory, const string_t& ok_text = "OK", const string_t& cancel_text = "Cancel", ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);
		static void delete_folder_dialog(const string_t& field_text, const string_t& field_desc, bool* is_open, const vec2_t& position, const vec2_t& size, const string_t& directory, const string_t& ok_text = "OK", const string_t& cancel_text = "Cancel");
		static bool image_button(const string_t& id, const raylib::Texture* texture, const vec2_t& size, const core::scolor& bg = {0, 0, 0, 255}, const core::scolor& tint = {250, 250, 250, 255});
		static void image(const raylib::Texture* texture, const vec2_t& size, const core::scolor& bg = {0, 0, 0, 255}, const core::scolor& tint = {250, 250, 250, 255});
		//- Rendering render target is possible with uv0 = 0.0f, 1.0f and uv1 = 1.0f, 0.0f
		static void image(const raylib::Texture* texture, const vec2_t& size, const vec2_t& uv0, const vec2_t& uv1, const core::scolor& bg = { 0, 0, 0, 255 }, const core::scolor& tint = { 250, 250, 250, 255 });
		static void toggle_button(const string_t& field_text, bool* v);
		static void toggle_button_ex(const string_t field_text, bool* v, const core::scolor& bg_active = {25, 250, 0, 255}, const core::scolor& bg_inactive = {150, 150, 150, 255}, const core::scolor& fg_knob = {250, 250, 250, 255});
		static void loading_bar(const string_t& field_id, float percent_value, const vec2_t& size, const core::scolor& background = {50, 50, 50, 255}, const core::scolor& foreground = {0, 150, 0, 255});
		static void loading_circle(const string_t& field_id, float radius, float thikness, const core::scolor& color = {0, 150, 0, 255});
		static bool edit_field_sint32(int& value, int min, int max, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags = ImGuiSliderFlags_None);
		static bool edit_field_sint64(long& value, long min, long max, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags = ImGuiSliderFlags_None);
		static bool edit_field_uint32(unsigned& value, unsigned min, unsigned max, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags = ImGuiSliderFlags_None);
		static bool edit_field_uint64(unsigned long& value, unsigned long min, unsigned long max, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags = ImGuiSliderFlags_None);
		static bool edit_field_f32(float& value, float min, float max, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags = ImGuiSliderFlags_None, float speed = 1.0f);
		static bool edit_field_bool(bool& value, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags = ImGuiSliderFlags_None);
		static bool edit_field_vec2(vec2_t& value, float min, float max, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags = ImGuiSliderFlags_None, float speed = 1.0f);
		static bool edit_field_vec3(vec3_t& value, float min, float max, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags = ImGuiSliderFlags_None, float speed = 1.0f);
		static bool edit_field_vec4(vec4_t& value, float min, float max, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags = ImGuiSliderFlags_None, float speed = 1.0f, bool color_wheel = false);
		static bool edit_field_string(string_t& value, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags = ImGuiSliderFlags_None);
		static bool edit_field_color(core::scolor& value, unsigned char min, unsigned char max, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags = ImGuiSliderFlags_None, bool color_wheel = true);
		static bool edit_field_rect(core::srect& value, float min, float max, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags = ImGuiSliderFlags_None, float speed = 1.0f);
		static bool edit_field_vec2_ranged(vec2_t& value, float min, float max, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags = ImGuiSliderFlags_None, float speed = 1.0f);
		static void help_marker(const string_t& desc);
		static void help_marker_no_question_mark(const string_t& desc, tooltip_hovering_delay delay = tooltip_hovering_delay_none);
		static bool ui_has_focus();

	private:
		struct InputTextCallback_UserData
		{
			string_t* Str;
			ImGuiInputTextCallback  ChainCallback;
			void* ChainCallbackUserData;
		};
		static int InputTextCallback(ImGuiInputTextCallbackData* data);
		static bool input_text_ex(const char* label, string_t* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data);
		static void show_notifications();

		static ImNodes::EditorContext* s_nodeeditor_context;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct snotification
	{
		static constexpr ImVec4 C_SUCCESS_COLOR = { 0.0f, 0.75f, 0.0025f, 1.0f };
		static constexpr ImVec4 C_INFO_COLOR = { 0.0f, 0.5f, 1.0f, 1.0f };
		static constexpr ImVec4 C_WARN_COLOR = { 1.0f, 1.0f, 0.0f, 1.0f };
		static constexpr ImVec4 C_ERROR_COLOR = { 1.0f, 0.75f, 0.0f, 1.0f };
		static constexpr ImVec4 C_CRITICAL_COLOR = { 0.75f, 0.0f, 0.0f, 1.0f };
		static constexpr unsigned C_SUCCESS_LIFETIME = 10;
		static constexpr unsigned C_INFO_LIFETIME = 13;
		static constexpr unsigned C_WARN_LIFETIME = 15;
		static constexpr unsigned C_ERROR_LIFETIME = 17;
		static constexpr unsigned C_CRITICAL_LIFETIME = 20;

		string_t m_label;
		string_t m_message;
		vec2_t m_position;
		vec2_t m_size;
		float m_lifetime_falloff;
		float m_lifetime;
		notification_type m_type;
	};
}