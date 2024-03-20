#pragma once
#include "sdl_service.hpp"

namespace sm
{
	//- mapping most important keys from SDL. Other keys from SDL can be checked directly.
	//------------------------------------------------------------------------------------------------------------------------
	enum keyboardkey : uint32_t
	{
		keyboardkey_none = 0,
		keyboardkey_a = SDLK_a,
		keyboardkey_b = SDLK_b,
		keyboardkey_c = SDLK_c,
		keyboardkey_d = SDLK_d,
		keyboardkey_e = SDLK_e,
		keyboardkey_f = SDLK_f,
		keyboardkey_g = SDLK_g,
		keyboardkey_h = SDLK_h,
		keyboardkey_i = SDLK_i,
		keyboardkey_j = SDLK_j,
		keyboardkey_k = SDLK_k,
		keyboardkey_l = SDLK_l,
		keyboardkey_m = SDLK_m,
		keyboardkey_n = SDLK_n,
		keyboardkey_o = SDLK_o,
		keyboardkey_p = SDLK_p,
		keyboardkey_q = SDLK_q,
		keyboardkey_r = SDLK_r,
		keyboardkey_s = SDLK_s,
		keyboardkey_t = SDLK_t,
		keyboardkey_u = SDLK_u,
		keyboardkey_v = SDLK_v,
		keyboardkey_w = SDLK_w,
		keyboardkey_x = SDLK_x,
		keyboardkey_y = SDLK_y,
		keyboardkey_z = SDLK_z,
		keyboardkey_leftbracket = SDLK_LEFTBRACKET,
		keyboardkey_backslash = SDLK_BACKSLASH,
		keyboardkey_rightbracket = SDLK_RIGHTBRACKET,
		keyboardkey_caret = SDLK_CARET,
		keyboardkey_underscore = SDLK_UNDERSCORE,
		keyboardkey_backquote = SDLK_BACKQUOTE,
		keyboardkey_return = SDLK_BACKQUOTE,
		keyboardkey_escape = SDLK_BACKQUOTE,
		keyboardkey_backspace = SDLK_BACKQUOTE,
		keyboardkey_tab = SDLK_BACKQUOTE,
		keyboardkey_exclaim = SDLK_BACKQUOTE,
		keyboardkey_space = SDLK_BACKQUOTE,
		keyboardkey_quotedbl = SDLK_BACKQUOTE,
		keyboardkey_hash = SDLK_BACKQUOTE,
		keyboardkey_percent = SDLK_BACKQUOTE,
		keyboardkey_dollar = SDLK_BACKQUOTE,
		keyboardkey_ampersand = SDLK_BACKQUOTE,
		keyboardkey_quote = SDLK_BACKQUOTE,
		keyboardkey_leftparen = SDLK_BACKQUOTE,
		keyboardkey_rightparen = SDLK_BACKQUOTE,
		keyboardkey_asterisk = SDLK_BACKQUOTE,
		keyboardkey_plus = SDLK_BACKQUOTE,
		keyboardkey_comma = SDLK_BACKQUOTE,
		keyboardkey_minus = SDLK_BACKQUOTE,
		keyboardkey_period = SDLK_BACKQUOTE,
		keyboardkey_slash = SDLK_BACKQUOTE,
		keyboardkey_0 = SDLK_BACKQUOTE,
		keyboardkey_1 = SDLK_BACKQUOTE,
		keyboardkey_2 = SDLK_BACKQUOTE,
		keyboardkey_3 = SDLK_BACKQUOTE,
		keyboardkey_4 = SDLK_BACKQUOTE,
		keyboardkey_5 = SDLK_BACKQUOTE,
		keyboardkey_6 = SDLK_BACKQUOTE,
		keyboardkey_7 = SDLK_BACKQUOTE,
		keyboardkey_8 = SDLK_BACKQUOTE,
		keyboardkey_9 = SDLK_BACKQUOTE,
		keyboardkey_colon = SDLK_BACKQUOTE,
		keyboardkey_semicolon = SDLK_BACKQUOTE,
		keyboardkey_less = SDLK_BACKQUOTE,
		keyboardkey_equals = SDLK_BACKQUOTE,
		keyboardkey_greater = SDLK_BACKQUOTE,
		keyboardkey_question = SDLK_BACKQUOTE,
		keyboardkey_at = SDLK_BACKQUOTE,
		keyboardkey_f1 = SDLK_F1,
		keyboardkey_f2 = SDLK_F2,
		keyboardkey_f3 = SDLK_F3,
		keyboardkey_f4 = SDLK_F4,
		keyboardkey_f5 = SDLK_F5,
		keyboardkey_f6 = SDLK_F6,
		keyboardkey_f7 = SDLK_F7,
		keyboardkey_f8 = SDLK_F8,
		keyboardkey_f9 = SDLK_F9,
		keyboardkey_f10 = SDLK_F10,
		keyboardkey_f11 = SDLK_F11,
		keyboardkey_f12 = SDLK_F12,
		keyboardkey_printscreen = SDLK_PRINTSCREEN,
		keyboardkey_scrollock = SDLK_SCROLLLOCK,
		keyboardkey_pause = SDLK_PAUSE,
		keyboardkey_insert = SDLK_INSERT,
		keyboardkey_home = SDLK_HOME,
		keyboardkey_pageup = SDLK_PAGEUP,
		keyboardkey_delete = SDLK_DELETE,
		keyboardkey_end = SDLK_END,
		keyboardkey_pagedown = SDLK_PAGEDOWN,
		keyboardkey_right = SDLK_RIGHT,
		keyboardkey_left = SDLK_LEFT,
		keyboardkey_down = SDLK_DOWN,
		keyboardkey_up = SDLK_UP
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum mousebutton : uint8_t
	{
		mousebutton_none	= 0,
		mousebutton_left	= SDL_BUTTON_LEFT,
		mousebutton_right	= SDL_BUTTON_RIGHT,
		mousebutton_middle	= SDL_BUTTON_MIDDLE,
		mousebutton_extra1	= SDL_BUTTON_X1,
		mousebutton_extra2	= SDL_BUTTON_X2,
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cinput_service final : public core::cservice
	{
	public:
		cinput_service() = default;
		~cinput_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float dt) override final;

		bool is_key_pressed(keyboardkey k);
		bool is_key_released(keyboardkey k);
		bool is_key_held(keyboardkey k);

		bool is_mouse_pressed(mousebutton b);
		bool is_mouse_released(mousebutton b);
		bool is_mouse_held(mousebutton b);

	private:
		using keyboard_state_t = vector_t<bool>;
		using mouse_state_t = array_t<bool, 6>;

		struct skeyboard
		{
			keyboard_state_t m_current;
			keyboard_state_t m_previous;
		};

		struct smouse
		{
			int m_x = 0, m_y = 0;
			mouse_state_t m_current;
			mouse_state_t m_previous;
		};

		skeyboard m_keyboard_state;
		core::cmutex m_mutex;
		smouse m_mouse_state;

		RTTR_ENABLE(core::cservice);

	private:
		void update_keyboard();
		void update_mouse();

	};

} //- sm

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cinput_service)
	{
		rttr::registration::class_<cinput_service>("cinput_service")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

} //- sm