#include "imgui.hpp"
#include "../sm_config.hpp"
#include "imgui/imgui_impl_bgfx.h"

namespace imgui
{
	namespace
	{
		constexpr float C_EDIT_DRAG_FIELD_WIDTH = 150.0f;
		constexpr float C_EDIT_SCALAR_FIELD_WIDTH = 250.0f;
		static inline vector_t< snotification > S_NOTIFICATIONS;
		static bx::DefaultAllocator S_ALLOCATOR;
		static ImGuiContext* S_IMGUI_CONTEXT = nullptr;
		static ImPlotContext* S_IMPLOT_CONTEXT = nullptr;

		//------------------------------------------------------------------------------------------------------------------------
		inline static ImTextureID bgfx_to_imgui_texture_id(bgfx::TextureHandle handle, uint8_t flags, uint8_t mip)
		{
			union { struct { bgfx::TextureHandle m_handle; uint8_t m_flags; uint8_t m_mip; } s; ImTextureID m_id; } tex;
			tex.s.m_handle = handle;
			tex.s.m_flags = flags;
			tex.s.m_mip = mip;
			return tex.m_id;
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static ImGuiKey core_key_to_imgui(core::key k)
		{
			switch (k)
			{
			default:
			case core::key_none: return ImGuiKey_None;
			case core::key_esc: return ImGuiKey_Escape;
			case core::key_return: return ImGuiKey_Enter;
			case core::key_tab: return ImGuiKey_Tab;
			case core::key_space: return ImGuiKey_Space;
			case core::key_backspace: return ImGuiKey_Backspace;
			case core::key_up: return ImGuiKey_UpArrow;
			case core::key_down: return ImGuiKey_DownArrow;
			case core::key_left: return ImGuiKey_LeftArrow;
			case core::key_right: return ImGuiKey_RightArrow;
			case core::key_insert: return ImGuiKey_Insert;
			case core::key_delete: return ImGuiKey_Delete;
			case core::key_home: return ImGuiKey_Home;
			case core::key_end: return ImGuiKey_End;
			case core::key_page_up: return ImGuiKey_PageUp;
			case core::key_page_down: return ImGuiKey_PageDown;
			case core::key_print: return ImGuiKey_PrintScreen;
			/*case core::key_plus: return ImGuiKey_Plus;*/
			case core::key_minus: return ImGuiKey_Minus;
			case core::key_left_bracket: return ImGuiKey_LeftBracket;
			case core::key_right_bracket: return ImGuiKey_RightBracket;
			case core::key_semicolon: return ImGuiKey_Semicolon;
			case core::key_quote: return ImGuiKey_Apostrophe;
			case core::key_comma: return ImGuiKey_Comma;
			case core::key_period: return ImGuiKey_Period;
			case core::key_slash: return ImGuiKey_Slash;
			case core::key_backslash: return ImGuiKey_Backslash;
			case core::key_tilde: return ImGuiKey_GraveAccent;
			case core::key_f1: return ImGuiKey_F1;
			case core::key_f2: return ImGuiKey_F2;
			case core::key_f3: return ImGuiKey_F3;
			case core::key_f4: return ImGuiKey_F4;
			case core::key_f5: return ImGuiKey_F5;
			case core::key_f6: return ImGuiKey_F6;
			case core::key_f7: return ImGuiKey_F7;
			case core::key_f8: return ImGuiKey_F8;
			case core::key_f9: return ImGuiKey_F9;
			case core::key_f10: return ImGuiKey_F10;
			case core::key_f11: return ImGuiKey_F11;
			case core::key_f12: return ImGuiKey_F12;
			case core::key_numpad0: return ImGuiKey_Keypad0;
			case core::key_numpad1: return ImGuiKey_Keypad1;
			case core::key_numpad2: return ImGuiKey_Keypad2;
			case core::key_numpad3: return ImGuiKey_Keypad3;
			case core::key_numpad4: return ImGuiKey_Keypad4;
			case core::key_numpad5: return ImGuiKey_Keypad5;
			case core::key_numpad6: return ImGuiKey_Keypad6;
			case core::key_numpad7: return ImGuiKey_Keypad7;
			case core::key_numpad8: return ImGuiKey_Keypad8;
			case core::key_numpad9: return ImGuiKey_Keypad9;
			case core::key_0: return ImGuiKey_0;
			case core::key_1: return ImGuiKey_1;
			case core::key_2: return ImGuiKey_2;
			case core::key_3: return ImGuiKey_3;
			case core::key_4: return ImGuiKey_4;
			case core::key_5: return ImGuiKey_5;
			case core::key_6: return ImGuiKey_6;
			case core::key_7: return ImGuiKey_7;
			case core::key_8: return ImGuiKey_8;
			case core::key_9: return ImGuiKey_9;
			case core::key_a: return ImGuiKey_A;
			case core::key_b: return ImGuiKey_B;
			case core::key_c: return ImGuiKey_C;
			case core::key_d: return ImGuiKey_D;
			case core::key_e: return ImGuiKey_E;
			case core::key_f: return ImGuiKey_F;
			case core::key_g: return ImGuiKey_G;
			case core::key_h: return ImGuiKey_H;
			case core::key_i: return ImGuiKey_I;
			case core::key_k: return ImGuiKey_K;
			case core::key_l: return ImGuiKey_L;
			case core::key_m: return ImGuiKey_M;
			case core::key_n: return ImGuiKey_N;
			case core::key_o: return ImGuiKey_O;
			case core::key_p: return ImGuiKey_P;
			case core::key_q: return ImGuiKey_Q;
			case core::key_r: return ImGuiKey_R;
			case core::key_s: return ImGuiKey_S;
			case core::key_t: return ImGuiKey_T;
			case core::key_u: return ImGuiKey_U;
			case core::key_v: return ImGuiKey_V;
			case core::key_w: return ImGuiKey_W;
			case core::key_x: return ImGuiKey_X;
			case core::key_y: return ImGuiKey_Y;
			case core::key_z: return ImGuiKey_Z;
			case core::key_gamepad_a: return ImGuiKey_GamepadFaceDown;
			case core::key_gamepad_b: return ImGuiKey_GamepadFaceRight;
			case core::key_gamepad_x: return ImGuiKey_GamepadFaceLeft;
			case core::key_gamepad_y: return ImGuiKey_GamepadFaceUp;
			case core::key_gamepad_thumb_L: return ImGuiKey_GamepadL3;
			case core::key_gamepad_thumb_R: return ImGuiKey_GamepadR3;
			case core::key_gamepad_shoulder_L: return ImGuiKey_GamepadL1;
			case core::key_gamepad_shoulder_R: return ImGuiKey_GamepadR1;
			case core::key_gamepad_up: return ImGuiKey_GamepadDpadUp;
			case core::key_gamepad_down: return ImGuiKey_GamepadDpadDown;
			case core::key_gamepad_left: return ImGuiKey_GamepadDpadLeft;
			case core::key_gamepad_right: return ImGuiKey_GamepadDpadRight;
			case core::key_gamepad_back: return ImGuiKey_GamepadBack;
			case core::key_gamepad_start: return ImGuiKey_GamepadStart;
			case core::key_gamepad_guide: return ImGuiKey_GamepadStart;
			}
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool init()
	{
		imgui_ctx();
		implot_ctx();
		imgui::imguiCreate(16.0f, &S_ALLOCATOR);

		
		//- setup default style
		ImGui::GetStyle().WindowRounding = 0;
		ImGui::GetStyle().ChildRounding = 0;
		ImGui::GetStyle().FrameRounding = 0;
		ImGui::GetStyle().PopupRounding = 0;
		ImGui::GetStyle().ScrollbarRounding = 0;
		ImGui::GetStyle().GrabRounding = 0;
		ImGui::GetStyle().TabRounding = 0;

		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable
			| ImGuiConfigFlags_ViewportsEnable
			| ImGuiConfigFlags_NavEnableGamepad
			| ImGuiConfigFlags_NavEnableKeyboard;

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void shutdown()
	{
		imgui::imguiDestroy();
		/*rlImGuiShutdown();*/
		ImPlot::DestroyContext(S_IMPLOT_CONTEXT);
		ImGui::DestroyContext(S_IMGUI_CONTEXT);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void begin()
	{
		if (auto os = sm::entry::get_os(); os)
		{
			static int64_t S_LAST = 0;
			ImGuiIO& io = ImGui::GetIO();
			double mx = 0.0, my = 0.0, sx = 0.0, sy = 0.0;
			os->mouse_position(&mx, &my);
			os->mouse_scroll_dt(&sx, &sy);

			const int64_t now = bx::getHPCounter();
			const int64_t frameTime = now - S_LAST;
			S_LAST = now;
			const double freq = double(bx::getHPFrequency());
			io.DeltaTime = float(frameTime / freq);

			if (const auto c = os->read_input_character(); c != '\0')
			{
				io.AddInputCharacter((unsigned)c);
			}

			//- Provide mouse events to ImGui
			io.AddMousePosEvent(SCAST(float, mx), SCAST(float, my));
			io.AddMouseButtonEvent(ImGuiMouseButton_Left, os->is_mouse_button_pressed(core::mouse_button_left) ||
				os->is_mouse_button_held(core::mouse_button_left));
			io.AddMouseButtonEvent(ImGuiMouseButton_Middle, os->is_mouse_button_pressed(core::mouse_button_middle) ||
				os->is_mouse_button_held(core::mouse_button_middle));
			io.AddMouseButtonEvent(ImGuiMouseButton_Right, os->is_mouse_button_pressed(core::mouse_button_right) ||
				os->is_mouse_button_held(core::mouse_button_right));
			io.AddMouseWheelEvent(sx, sy);

			//- Provide keyboard events to ImGui
			io.AddKeyEvent(ImGuiMod_Shift, os->is_modifier_active(core::key_modifier_left_shift | core::key_modifier_right_shift));
			io.AddKeyEvent(ImGuiMod_Ctrl, os->is_modifier_active(core::key_modifier_left_ctrl | core::key_modifier_right_ctrl));
			io.AddKeyEvent(ImGuiMod_Alt, os->is_modifier_active(core::key_modifier_left_alt | core::key_modifier_right_alt));
			io.AddKeyEvent(ImGuiMod_Super, os->is_modifier_active(core::key_modifier_left_meta | core::key_modifier_right_meta));

			for (int i = 0; i < (int)core::key::key_count; ++i)
			{
				const auto pressed_or_held = os->is_key_held((core::key)i)
					|| os->is_key_pressed((core::key)i);

				io.AddKeyEvent(core_key_to_imgui((core::key)i), pressed_or_held);
			}

			ImGui::NewFrame();
		}
		/*rlImGuiBegin();*/
	}

	//------------------------------------------------------------------------------------------------------------------------
	void end()
	{
		imgui::imguiEndFrame();
		/*rlImGuiEnd();*/
	}

	//------------------------------------------------------------------------------------------------------------------------
	ImGuiContext* imgui_ctx()
	{
		if (!S_IMGUI_CONTEXT)
		{
			S_IMGUI_CONTEXT = ImGui::CreateContext();
		}
		return S_IMGUI_CONTEXT;
	}

	//------------------------------------------------------------------------------------------------------------------------
	ImPlotContext* implot_ctx()
	{
		if (!S_IMPLOT_CONTEXT)
		{
			S_IMPLOT_CONTEXT = ImPlot::CreateContext();
		}
		return S_IMPLOT_CONTEXT;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cui::on_resize(unsigned w, unsigned h)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize.x = float(w);
		io.DisplaySize.y = float(h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cui::get_imgui_color(float r, float g, float b, float a)
	{
		return ImGui::ColorConvertFloat4ToU32({ r, g, b, a });
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cui::get_imgui_color(const core::scolor& color)
	{
		auto v = color.normalize();
		return get_imgui_color(v.x, v.y, v.z, v.w);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cui::create_notification(const string_t& label, const string_t& message, notification_type type,
		unsigned lifetime_seconds/* = 0*/, const vec2_t& size /*= { 400.0f, 100.0f }*/)
	{
		if (lifetime_seconds == 0)
		{
			switch (type)
			{
			case notification_type_warn:
			{
				lifetime_seconds = snotification::s_warn_lifetime;
				break;
			}
			case notification_type_error:
			{
				lifetime_seconds = snotification::s_error_lifetime;
				break;
			}
			case notification_type_critical:
			{
				lifetime_seconds = snotification::s_critical_lifetime;
				break;
			}
			case notification_type_success:
			{
				lifetime_seconds = snotification::s_success_lifetime;
				break;
			}
			default:
			case notification_type_none:
			case notification_type_info:
			{
				lifetime_seconds = snotification::s_info_lifetime;
				break;
			}
			}
		}

		ImGuiIO& io = ImGui::GetIO();

		S_NOTIFICATIONS.emplace_back(snotification{ type, label, message, { io.DisplaySize.x - size.x - 10.0f, io.DisplaySize.y - size.y - 10.0f }, size, 1.0f / (SCAST(float, lifetime_seconds) * 100.0f), 1.0f });
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cui::create_file_dialog(const string_t& field_text, const string_t& field_desc, bool* is_open,
		const vec2_t& position, const vec2_t& size, const char* allowed_file_types[], unsigned file_type_count, const string_t& directory,
		const string_t& ok_text /*= "OK"*/, const string_t& cancel_text /*= "Cancel"*/, ImGuiInputTextFlags flags /*= ImGuiInputTextFlags_None*/)
	{
		static const char* selected_file_type;
		static int selected_file_type_option = -1;
		static char buffer[128];

		ImGui::SetNextWindowPos({ position.x, position.y }, ImGuiCond_Appearing);
		ImGui::SetNextWindowSize({ size.x, size.y }, ImGuiCond_Appearing);

		ImGui::Begin(field_text.c_str(), is_open);

		ImGui::InputText("## File_Naming_Textinput", &buffer[0], sizeof(buffer), flags);

		ImGui::SameLine();

		if (ImGui::SmallButton(selected_file_type_option == -1 ? "Please select a Filetype" : selected_file_type))
		{
			ImGui::OpenPopup("File_Type_Selection_Popup");
		}

		cui::help_marker_no_question_mark(field_desc);


		if (ImGui::BeginPopup("File_Type_Selection_Popup"))
		{
			for (int i = 0; i < file_type_count; i++)
			{
				if (ImGui::Selectable(allowed_file_types[i])) { selected_file_type_option = i; }
			}

			selected_file_type = allowed_file_types[selected_file_type_option];

			ImGui::EndPopup();
		}

		if (ImGui::SmallButton(ok_text.c_str()))
		{
			// Get the string from text input buffer.
			string_t file_name = string_t(buffer);

			// Sanity check for the file and that type was selected.
			if (!file_name.empty() && selected_file_type_option >= 0 && selected_file_type_option < file_type_count)
			{
				// Ready to create file. Get the extension from allowed file types.
				string_t extension = string_t(allowed_file_types[selected_file_type_option]);

				// Try creating the file.
				if (core::cfilesystem::create_file_in(directory.data(), file_name.data(), extension.data()))
				{
					cui::create_notification("Success", fmt::format("Success creating file \"%s\" at \"%s\"!", string_t(file_name + extension).c_str(), directory.c_str()), notification_type_success, snotification::s_success_lifetime);
				}
				else
				{
					cui::create_notification("Failure", fmt::format("Failed creating file \"%s\" at \"%s\"!", string_t(file_name + extension).c_str(), directory.c_str()), notification_type_error, snotification::s_error_lifetime);
				}

				// Reset the internal state.
				*is_open = false;
				selected_file_type_option = -1;
				std::memset(&buffer, NULL, sizeof(buffer));
			}
		}

		ImGui::SameLine();

		if (ImGui::SmallButton(cancel_text.c_str()))
		{
			// Reset the internal state.
			*is_open = false;
			selected_file_type_option = -1;
			std::memset(&buffer, NULL, sizeof(buffer));
		}

		ImGui::End();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cui::create_folder_dialog(const string_t& field_text, const string_t& field_desc, bool* is_open,
		const vec2_t& position, const vec2_t& size, const string_t& directory, const string_t& ok_text /*= "OK"*/, const string_t& cancel_text /*= "Cancel"*/, ImGuiInputTextFlags flags /*= ImGuiInputTextFlags_None*/)
	{
		static char buffer[128];

		ImGui::SetNextWindowPos({ position.x, position.y }, ImGuiCond_Appearing);
		ImGui::SetNextWindowSize({ size.x, size.y }, ImGuiCond_Appearing);

		ImGui::Begin(field_text.c_str(), is_open);


		ImGui::InputText("## Folder_Naming_Textinput", &buffer[0], sizeof(buffer), flags);


		if (ImGui::SmallButton(ok_text.c_str()))
		{
			// Get the string from text input buffer.
			string_t folder_name = string_t(buffer);

			if (!folder_name.empty())
			{
				// Try creating folder with specified name at specified location.
				if (core::cfilesystem::create_dir(core::cfilesystem::construct(directory.data(), folder_name.data()).view()))
				{
					cui::create_notification("Success", fmt::format("Success creating folder \"%s\" at \"%s\"!", folder_name.c_str(), directory.c_str()), notification_type_success, snotification::s_success_lifetime);
				}
				else
				{
					cui::create_notification("Failure", fmt::format("Failed creating folder \"%s\" at \"%s\"!", folder_name.c_str(), directory.c_str()), notification_type_error, snotification::s_error_lifetime);
				}

				// Reset the internal state.
				*is_open = false;
				std::memset(&buffer, NULL, sizeof(buffer));
			}
		}

		ImGui::SameLine();

		if (ImGui::SmallButton(cancel_text.c_str()))
		{
			// Reset the internal state.
			*is_open = false;
			std::memset(&buffer, NULL, sizeof(buffer));
		}

		ImGui::End();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cui::delete_folder_dialog(const string_t& field_text, const string_t& field_desc, bool* is_open,
		const vec2_t& position, const vec2_t& size, const string_t& directory, const string_t& ok_text /*= "OK"*/, const string_t& cancel_text /*= "Cancel"*/)
	{
		ImGui::SetNextWindowPos({ position.x, position.y }, ImGuiCond_Appearing);
		ImGui::SetNextWindowSize({ size.x, size.y }, ImGuiCond_Appearing);

		ImGui::Begin(field_text.c_str(), is_open);

		if (ImGui::SmallButton(ok_text.c_str()))
		{
			if (core::cfilesystem::remove(directory.data()))
			{
				cui::create_notification("Success", fmt::format("Success deleting folder \"%s\"!", directory.c_str()), notification_type_success, snotification::s_success_lifetime);
			}
			else
			{
				cui::create_notification("Failure", fmt::format("Failed deleting folder \"%s\"!", directory.c_str()), notification_type_error, snotification::s_error_lifetime);
			}

			// Reset the internal state.
			*is_open = false;
		}

		ImGui::SameLine();

		if (ImGui::SmallButton(cancel_text.c_str()))
		{
			// Reset the internal state.
			*is_open = false;
		}

		ImGui::End();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::image_button(const string_t& id, const bgfx::TextureHandle texture, const vec2_t& size,
		const core::scolor& bg /*= BLACK*/, const core::scolor& tint /*= RAYWHITE*/)
	{
		auto bg4 = bg.normalize();
		auto tint4 = tint.normalize();

		ImGui::PushStyleColor(ImGuiCol_Button, { bg4.r, bg4.g, bg4.b, 0.0f });

		bool result = ImGui::ImageButton(id.c_str(), bgfx_to_imgui_texture_id(texture, 0, 0), {size.x, size.y},
			{ 0.0f, 0.0f }, { 1.0f, 1.0f }, { bg4.r, bg4.g, bg4.b, bg4.a }, { tint4.r, tint4.g, tint4.b, tint4.a });

		ImGui::PopStyleColor();
		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cui::image(const bgfx::TextureHandle texture, const vec2_t& size,
		const core::scolor& bg /*= BLACK*/, const core::scolor& tint /*= RAYWHITE*/)
	{
		image(texture, size, { 0.0f, 0.0f }, { 1.0f, 1.0f }, bg, tint);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cui::image(const bgfx::TextureHandle texture, const vec2_t& size, const vec2_t& uv0, const vec2_t& uv1,
		const core::scolor& bg /*= { 0, 0, 0, 255 }*/, const core::scolor& tint /*= { 250, 250, 250, 255 }*/)
	{
		auto bg4 = bg.normalize();
		auto tint4 = tint.normalize();

		ImGui::PushStyleColor(ImGuiCol_Button, { bg4.r, bg4.g, bg4.b, 0.0f });

		ImGui::Image(bgfx_to_imgui_texture_id(texture, 0, 0), { size.x, size.y },
			{ uv0.x, uv0.y }, { uv1.x, uv1.y }, { bg4.r, bg4.g, bg4.b, bg4.a }, { tint4.r, tint4.g, tint4.b, tint4.a });

		ImGui::PopStyleColor();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cui::toggle_button(const string_t& field_text, bool* v)
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		float height = ImGui::GetFrameHeight();
		float width = height * 1.55f;
		float radius = height * 0.50f;

		ImGui::InvisibleButton(field_text.c_str(), ImVec2(width, height));
		if (ImGui::IsItemClicked())
			*v = !*v;

		float t = *v ? 1.0f : 0.0f;

		ImGuiContext& g = *GImGui;
		float ANIM_SPEED = 0.08f;
		if (g.LastActiveId == g.CurrentWindow->GetID(field_text.c_str()))// && g.LastActiveIdTimer < ANIM_SPEED)
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (t_anim) : (1.0f - t_anim);
		}

		unsigned col_bg;
		if (ImGui::IsItemHovered())
			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
		else
			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
		draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cui::toggle_button_ex(const string_t field_text, bool* v, const core::scolor& bg_active /*= LIME*/,
		const core::scolor& bg_inactive /*= LIGHTGRAY*/, const core::scolor& fg_knob /*= RAYWHITE*/)
	{
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		float height = ImGui::GetFrameHeight();
		float width = height * 1.55f;
		float radius = height * 0.50f;

		ImGui::InvisibleButton(field_text.c_str(), ImVec2(width, height));
		if (ImGui::IsItemClicked())
			*v = !*v;

		float t = *v ? 1.0f : 0.0f;

		ImGuiContext& g = *GImGui;
		float ANIM_SPEED = 0.08f;
		if (g.LastActiveId == g.CurrentWindow->GetID(field_text.c_str()))// && g.LastActiveIdTimer < ANIM_SPEED)
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (t_anim) : (1.0f - t_anim);
		}

		auto bginactivevec = bg_inactive.normalize();
		auto bgactivevec = bg_active.normalize();

		unsigned col_bg;
		if (ImGui::IsItemHovered())
			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(bginactivevec.r, bginactivevec.g, bginactivevec.b, bginactivevec.a - 0.2f), ImVec4(bgactivevec.r, bgactivevec.g, bgactivevec.b, bgactivevec.a - 0.2f), t));
		else
			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(bginactivevec.r, bginactivevec.g, bginactivevec.b, bginactivevec.a + 0.2f), ImVec4(bgactivevec.r, bgactivevec.g, bgactivevec.b, bgactivevec.a + 0.2f), t));

		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
		draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(fg_knob.m_r, fg_knob.m_g, fg_knob.m_b, fg_knob.m_a));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cui::loading_bar(const string_t& field_id, float percent_value, const vec2_t& size, const core::scolor& background /*= DARKGRAY*/,
		const core::scolor& foreground /*= GREEN*/)
	{
		auto bg = background.normalize();
		auto fg = foreground.normalize();
		float v = percent_value / 100.0f;
		ImGui::BufferingBar(field_id.c_str(), v, { size.x, size.y }, ImGui::GetColorU32(ImVec4(bg.r, bg.g, bg.b, bg.a)), ImGui::GetColorU32(ImVec4(fg.r, fg.g, fg.b, fg.a)));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cui::loading_circle(const string_t& field_id, float radius, float thikness, const core::scolor& color /*= GREEN*/)
	{
		auto col = color.normalize();
		ImGui::Spinner(field_id.c_str(), radius, thikness, ImGui::GetColorU32(ImVec4(col.x, col.y, col.z, col.w)));
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::edit_field_sint32(int& value, int min, int max, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags /*= ImGuiSliderFlags_None*/)
	{
		bool result = false;

		if (ImGui::CollapsingHeader(field_text.c_str()))
		{
			int prev = value;

			ImGui::PushID(slider_id);
			ImGui::PushItemWidth(C_EDIT_DRAG_FIELD_WIDTH);
			auto drag = ImGui::DragInt("int", &prev, 1, min, max, "%d", flags);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH);
			auto scalar = ImGui::InputInt("scalar", &prev, 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			value = prev;

			result = drag || scalar;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::edit_field_sint64(int64_t& value, int64_t min, int64_t max, const string_t& field_text, const string_t& field_desc,
		ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags /*= ImGuiSliderFlags_None*/)
	{
		bool result = false;

		if (ImGui::CollapsingHeader(field_text.c_str()))
		{
			int prev = value;

			ImGui::PushID(slider_id);
			ImGui::PushItemWidth(C_EDIT_DRAG_FIELD_WIDTH);
			auto drag = ImGui::DragInt("int64_t", &prev, 1, min, max, "%d", flags);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH);
			auto scalar = ImGui::InputInt("scalar", &prev, 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			value = prev;

			result = drag || scalar;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::edit_field_uint32(unsigned& value, unsigned min, unsigned max, const string_t& field_text, const string_t& field_desc,
		ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags /*= ImGuiSliderFlags_None*/)
	{
		bool result = false;

		if (ImGui::CollapsingHeader(field_text.c_str()))
		{
			int prev = value;

			ImGui::PushID(slider_id);
			ImGui::PushItemWidth(C_EDIT_DRAG_FIELD_WIDTH);
			auto drag = ImGui::DragInt("##unsigned", &prev, 1, min, max, "%d", flags);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH);
			auto scalar = ImGui::InputInt("##scalar", &prev, 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			value = prev;

			result = drag || scalar;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::edit_field_uint64(uint64_t& value, uint64_t min, uint64_t max, const string_t& field_text, const string_t& field_desc,
		ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags /*= ImGuiSliderFlags_None*/)
	{
		bool result = false;

		if (ImGui::CollapsingHeader(field_text.c_str()))
		{
			int prev = value;

			ImGui::PushID(slider_id);
			ImGui::PushItemWidth(C_EDIT_DRAG_FIELD_WIDTH);
			auto drag = ImGui::DragInt("uint64_t", &prev, 1, min, max, "%d", flags);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH);
			auto scalar = ImGui::InputInt("scalar", &prev, 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			value = prev;

			result = drag || scalar;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::edit_field_float(float& value, float min, float max, const string_t& field_text, const string_t& field_desc,
		ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags /*= ImGuiSliderFlags_None*/, float speed /*= 1.0f*/)
	{
		bool result = false;

		if (ImGui::CollapsingHeader(field_text.c_str()))
		{
			float prev = value;

			ImGui::PushID(slider_id);
			ImGui::PushItemWidth(C_EDIT_DRAG_FIELD_WIDTH);
			auto drag = ImGui::DragFloat("float", &prev, speed, min, max, "%.4f", flags);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH);
			auto scalar = ImGui::InputFloat("scalar", &prev, 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			value = prev;

			result = drag || scalar;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::edit_field_bool(bool& value, const string_t& field_text, const string_t& field_desc,
		ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags /*= ImGuiSliderFlags_None*/)
	{
		bool result = false;

		if (ImGui::CollapsingHeader(field_text.c_str()))
		{
			ImGui::PushID(slider_id);
			auto box = ImGui::Checkbox("boolean", &value);
			ImGui::PopID();
			help_marker_no_question_mark(field_desc);

			result = box;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::edit_field_vec2(vec2_t& value, float min, float max, const string_t& field_text, const string_t& field_desc,
		ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags /*= ImGuiSliderFlags_None*/, float speed /*= 1.0f*/)
	{
		bool result = false;

		float prev[2]; prev[0] = value.x; prev[1] = value.y;

		if (ImGui::CollapsingHeader(field_text.c_str()))
		{
			ImGui::PushID(slider_id);
			ImGui::PushItemWidth(C_EDIT_DRAG_FIELD_WIDTH /*250.0f*/);
			auto drag = ImGui::DragFloat2("float", prev, speed, min, max, "%.4f", flags);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);


			ImGui::PushID(scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto x = ImGui::InputFloat("X", &prev[0], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(++scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto y = ImGui::InputFloat("Y", &prev[1], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			value.x = prev[0];
			value.y = prev[1];

			result = drag || x || y;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::edit_field_vec3(vec3_t& value, float min, float max, const string_t& field_text, const string_t& field_desc,
		ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags /*= ImGuiSliderFlags_None*/, float speed /*= 1.0f*/)
	{
		bool result = false;

		float prev[3]; prev[0] = value.x; prev[1] = value.y; prev[2] = value.z;

		if (ImGui::CollapsingHeader(field_text.c_str()))
		{
			ImGui::PushID(slider_id);
			ImGui::PushItemWidth(C_EDIT_DRAG_FIELD_WIDTH /*250.0f*/);
			auto drag = ImGui::DragFloat3("float", prev, speed, min, max, "%.4f", flags);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);


			ImGui::PushID(scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto x = ImGui::InputFloat("X", &prev[0], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(++scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto y = ImGui::InputFloat("Y", &prev[1], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(++scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto z = ImGui::InputFloat("Z", &prev[2], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			value.x = prev[0];
			value.y = prev[1];
			value.z = prev[2];

			result = drag || x || y || z;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::edit_field_vec4(vec4_t& value, float min, float max, const string_t& field_text, const string_t& field_desc,
		ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags /*= ImGuiSliderFlags_None*/, float speed /*= 1.0f*/, bool color_wheel /*= false*/)
	{
		bool result = false;

		float prev[4]; prev[0] = value.x; prev[1] = value.y; prev[2] = value.z; prev[3] = value.w;

		if (ImGui::CollapsingHeader(field_text.c_str()))
		{
			ImGui::PushID(slider_id);
			ImGui::PushItemWidth(C_EDIT_DRAG_FIELD_WIDTH /*250.0f*/);
			auto drag = ImGui::DragFloat4("float", prev, speed, min, max, "%.4f", flags);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);


			bool color_edited = false;
			if (color_wheel)
			{
				ImGui::SameLine();

				float color[4] = { value.r / 255.0f, value.g / 255.0f, value.b / 255.0f, value.a / 255.0f };

				if (ImGui::ColorEdit4(fmt::format("##%s_Color_Picker", field_text).data(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar))
				{
					prev[0] = color[0] * 255;
					prev[1] = color[1] * 255;
					prev[2] = color[2] * 255;
					prev[3] = color[3] * 255;
					color_edited = true;
				}
			}


			ImGui::PushID(scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto x = ImGui::InputFloat("X", &prev[0], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(++scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto y = ImGui::InputFloat("Y", &prev[1], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(++scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto z = ImGui::InputFloat("Z", &prev[2], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(++scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto w = ImGui::InputFloat("W", &prev[3], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			value.x = prev[0];
			value.y = prev[1];
			value.z = prev[2];
			value.w = prev[3];

			result = drag || x || y || z || w || color_edited;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::edit_field_string(string_t& value, const string_t& field_text, const string_t& field_desc, ImGuiID slider_id,
		ImGuiID scalar_id, ImGuiSliderFlags flags /*= ImGuiSliderFlags_None*/)
	{
		bool result = false;

		string_t text = "##" + field_text;

		if (ImGui::CollapsingHeader(field_text.c_str()))
		{
			unsigned item_width = ImGui::CalcTextSize(value.c_str()).x;
			if (item_width < 75.0f) item_width = 75.0f;


			ImGui::PushID(slider_id);
			ImGui::PushItemWidth(item_width);

			auto input = input_text_ex(text.c_str(), &value, ImGuiInputTextFlags_None, InputTextCallback, nullptr);

			ImGui::PopID();
			ImGui::PopItemWidth();

			help_marker_no_question_mark(field_desc);

			result = input;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::edit_field_color(core::scolor& value, byte_t min, byte_t max, const string_t& field_text, const string_t& field_desc,
		ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags /*= ImGuiSliderFlags_None*/, bool color_wheel /*= true*/)
	{
		bool result = false;

		int prev[4]; prev[0] = value.m_r; prev[1] = value.m_g; prev[2] = value.m_b; prev[3] = value.m_a;

		if (ImGui::CollapsingHeader(field_text.c_str()))
		{
			ImGui::PushID(slider_id);
			ImGui::PushItemWidth(C_EDIT_DRAG_FIELD_WIDTH /*250.0f*/);
			auto drag = ImGui::DragInt4("u8", prev, 1.0f, min, max, "%d", flags);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);


			bool color_edited = false;
			if (color_wheel)
			{
				ImGui::SameLine();

				float color[4] = { value.m_r / 255.0f, value.m_g / 255.0f, value.m_b / 255.0f, value.m_a / 255.0f };

				if (ImGui::ColorEdit4(fmt::format("##%s_Color_Picker", field_text).data(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar))
				{
					prev[0] = color[0] * 255;
					prev[1] = color[1] * 255;
					prev[2] = color[2] * 255;
					prev[3] = color[3] * 255;
					color_edited = true;
				}
			}


			ImGui::PushID(scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto r = ImGui::InputInt("R", &prev[0], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(++scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto g = ImGui::InputInt("G", &prev[1], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(++scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto b = ImGui::InputInt("B", &prev[2], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(++scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto a = ImGui::InputInt("A", &prev[3], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			value.m_r = SCAST(byte_t, prev[0]);
			value.m_g = SCAST(byte_t, prev[1]);
			value.m_b = SCAST(byte_t, prev[2]);
			value.m_a = SCAST(byte_t, prev[3]);
			result = drag || r || g || b || a || color_edited;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::edit_field_rect(core::srect& value, float min, float max, const string_t& field_text, const string_t& field_desc,
		ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags /*= ImGuiSliderFlags_None*/, float speed /*= 1.0f*/)
	{
		bool result = false;

		float prev[4]; prev[0] = value.x(); prev[1] = value.y(); prev[2] = value.w(); prev[3] = value.h();

		if (ImGui::CollapsingHeader(field_text.c_str()))
		{
			ImGui::PushID(slider_id);
			ImGui::PushItemWidth(C_EDIT_DRAG_FIELD_WIDTH /*250.0f*/);
			auto drag = ImGui::DragFloat4("float", prev, speed, min, max, "%.4f", flags);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);


			ImGui::PushID(scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto x = ImGui::InputFloat("X", &prev[0], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(++scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto y = ImGui::InputFloat("Y", &prev[1], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(++scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto w = ImGui::InputFloat("W", &prev[2], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			ImGui::SameLine();

			ImGui::PushID(++scalar_id);
			ImGui::PushItemWidth(C_EDIT_SCALAR_FIELD_WIDTH /*125.0f*/);
			auto h = ImGui::InputFloat("H", &prev[3], 0, 0);
			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);

			value.set(prev[0], prev[1], prev[2], prev[3]);
			result = drag || x || y || w || h;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::edit_field_vec2_ranged(vec2_t& value, float min, float max, const string_t& field_text, const string_t& field_desc,
		ImGuiID slider_id, ImGuiID scalar_id, ImGuiSliderFlags flags /*= ImGuiSliderFlags_None*/, float speed /*= 1.0f*/)
	{
		bool result = false;

		float prev[2]; prev[0] = value.x; prev[1] = value.y;

		if (ImGui::CollapsingHeader(field_text.c_str()))
		{
			ImGui::PushID(slider_id);
			ImGui::PushItemWidth(2.0f * C_EDIT_DRAG_FIELD_WIDTH /*250.0f*/);

			auto drag = ImGui::DragFloatRange2("float", &prev[0], &prev[1], speed, min, max, "Min: %.4f", "Max: %.4f", flags);

			ImGui::PopID();
			ImGui::PopItemWidth();
			help_marker_no_question_mark(field_desc);


			value.x = prev[0];
			value.y = prev[1];

			result = drag;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cui::help_marker(const string_t& desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * desc.size());
			ImGui::TextUnformatted(desc.c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cui::help_marker_no_question_mark(const string_t& desc, tooltip_hovering_delay delay /*= tooltip_hovering_delay_none*/)
	{
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * desc.size());
			ImGui::TextUnformatted(desc.c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::ui_has_focus()
	{
		return ImGui::IsAnyItemHovered() || ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
	}

	//------------------------------------------------------------------------------------------------------------------------
	int cui::InputTextCallback(ImGuiInputTextCallbackData* data)
	{
		InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			// Resize string callback
			// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
			string_t* str = user_data->Str;
			IM_ASSERT(data->Buf == str->c_str());
			str->resize(data->BufTextLen);
			data->Buf = (char*)str->c_str();
		}
		else if (user_data->ChainCallback)
		{
			// Forward to user callback, if any
			data->UserData = user_data->ChainCallbackUserData;
			return user_data->ChainCallback(data);
		}
		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cui::input_text_ex(const char* label, string_t* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
	{
		IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
		flags |= ImGuiInputTextFlags_CallbackResize;

		InputTextCallback_UserData cb_user_data;
		cb_user_data.Str = str;
		cb_user_data.ChainCallback = callback;
		cb_user_data.ChainCallbackUserData = user_data;
		return ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cui::show_notifications(float dt)
	{
		if (!S_NOTIFICATIONS.empty())
		{
			ImGuiIO& io = ImGui::GetIO();
			vector_t< unsigned > to_be_removed;
			unsigned max_index = 0;
			float height_modifier = 0.0f;
			unsigned index = 0;
			string_t window_name;


			//- Compute max number of notifications we can display on the window.
			max_index = glm::min(unsigned(io.DisplaySize.y / S_NOTIFICATIONS[0].m_size.y), (unsigned)S_NOTIFICATIONS.size());

			for (auto i = 0; i < max_index; i++)
			{
				auto& notify = S_NOTIFICATIONS[i];

				window_name = fmt::format("##notification_window_{}", i).c_str();

				//- Construct the label for the Notification.
				string_t label;
				ImVec4 color;
				switch (notify.m_type)
				{
				case notification_type_warn:
				{
					label = ICON_KI_INFO;

					color = snotification::s_warn_color;
					break;
				}
				case notification_type_error:
				{
					label = ICON_KI_EXCLAMATION_CIRCLE;
					color = snotification::s_error_color;
					break;
				}
				case notification_type_critical:
				{
					label = ICON_KI_EXCLAMATION_TRIANGLE;
					color = snotification::s_critical_color;
					break;
				}
				case notification_type_success:
				{
					label = ICON_KI_CHECK;
					color = snotification::s_success_color;
					break;
				}
				default:
				case notification_type_none:
				case notification_type_info:
				{
					label = ICON_KI_INFO;
					color = snotification::s_info_color;
					break;
				}
				}

				label += " " + notify.m_label;

				//- Set window position and style data.
				ImGui::SetNextWindowPos({ notify.m_position.x, notify.m_position.y - height_modifier }, ImGuiCond_Always);
				ImGui::SetNextWindowSize({ notify.m_size.x, notify.m_size.y }, ImGuiCond_Always);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12);

				//- Begin notification logic.
				bool clicked = false;


				if (ImGui::Begin(window_name.c_str(), (bool*)nullptr, ImGuiWindowFlags_NoDecoration))
				{
					ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

					if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						clicked = true;
					}


					//- Draw the Notification label.
					ImGui::PushStyleColor(ImGuiCol_Text, color);

					ImGui::Text(label.c_str());

					ImGui::PopStyleColor();


					ImGui::Separator();


					//- Set wrapping for Text message, and draw it.
					ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + notify.m_size.x - 10.0f);

					ImGui::Text(notify.m_message.c_str());

					ImGui::PopTextWrapPos();
				}

				ImGui::End();

				ImGui::PopStyleVar();

				if (clicked)
				{
					//- Notification acknowledged. Remove.
					to_be_removed.push_back(i); clicked = false;
				}
				else if (i == 0)
				{
					//- Decrease lifetime in order.
					notify.m_lifetime -= (notify.m_lifetime_falloff * dt);

					if (notify.m_lifetime <= 0.0f)
					{
						to_be_removed.push_back(i);
					}
				}
				height_modifier += notify.m_size.y + 2.5f;
			}

			//- Remove notifications we dont need anymore.
			for (const auto& idx: to_be_removed)
			{
				algorithm::erase_at_index(S_NOTIFICATIONS, idx);
			}
		}
	}

} //- imgui

namespace ImGui
{
	//- reference: https://github.com/ocornut/imgui/issues/1901
	//------------------------------------------------------------------------------------------------------------------------
	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const unsigned& bg_col, const unsigned& fg_col)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = size_arg;
		size.x -= style.FramePadding.x * 2;

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		// Render
		const float circleStart = size.x * 0.7f;
		const float circleEnd = size.x;
		const float circleWidth = circleEnd - circleStart;

		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

		// Render Text on top of bar.
		const char* percent_text = fmt::format("{:d}/100", (int)(value * 100.0f)).c_str();
		const float percent_text_w = ImGui::CalcTextSize(percent_text).x;
		window->DrawList->AddText(ImVec2(bb.Min.x + size_arg.x / 2.0f - percent_text_w - percent_text_w / 2.0f, bb.Min.y), ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), percent_text);

		const float t = g.Time;
		const float r = size.y / 2;
		const float speed = 1.5f;

		const float a = speed * 0;
		const float b = speed * 0.333f;
		const float c = speed * 0.666f;

		const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
		const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
		const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool Spinner(const char* label, float radius, int thickness, const unsigned& color)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		//- render
		window->DrawList->PathClear();

		int num_segments = 30;
		int start = glm::abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

		const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
		const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

		const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

		for (int i = 0; i < num_segments; i++) {
			const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
			window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
				centre.y + ImSin(a + g.Time * 8) * radius));
		}

		window->DrawList->PathStroke(color, false, thickness);
	}

} //- ImGui