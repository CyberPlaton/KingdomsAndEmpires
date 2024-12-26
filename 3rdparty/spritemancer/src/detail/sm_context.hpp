#pragma once
#include "sm_config.hpp"

//------------------------------------------------------------------------------------------------------------------------
#define CTX_FLAG_FUNCTION(name, flag) \
void name(const bool value) \
{ \
    if (value) \
    { \
        algorithm::bit_set(m_flags, flag); \
    } \
    else \
    { \
        algorithm::bit_clear(m_flags, flag); \
    } \
} \
inline bool name() const { return check(flag); }

namespace sm
{

    namespace detail
    {
        //- Structure containing data relevant to hardware and input devices if necessary
        //------------------------------------------------------------------------------------------------------------------------
        struct sio
        {
            unsigned m_window_x = 0;
            unsigned m_window_y = 0;
            unsigned m_window_w = 0;
            unsigned m_window_h = 0;
            float m_dt = 0.0f;
            bool m_fullscreen = false;
            bool m_vsync = false;
        };
        
        //- Structure containing data relevant to GPU and drawing
        //------------------------------------------------------------------------------------------------------------------------
        struct sgraphics
        {
            program_handle_t m_shader = 0;
        };
    
    } //- detail

    //- Class responsible for storing the current state of the library and connecting distinct parts of it as a single point
    //- of interface.
    //------------------------------------------------------------------------------------------------------------------------
    class ccontext final
    {
    public:
        static constexpr auto C_DEFAULT_ROTATION    = 0.0f;
        static constexpr vec2_t C_DEFAULT_SCALE        = { 1.0f, 1.0f };
        static constexpr vec2_t C_DEFAULT_ORIGIN    = { 0.5f, 0.5f };
        static inline const auto C_DEFAULT_SOURCE    = core::srect{ 0.0f, 0.0f, 1.0f, 1.0f };
        static inline const core::scolor C_WHITE    = { 255, 255, 255, 255 };
        static inline const core::scolor C_BLACK    = { 0, 0, 0, 0 };
        static inline const core::scolor C_BLANK    = { 0, 0, 0, 255 };

        detail::sio& io();
        const detail::sio& io() const;

        detail::sgraphics& graphics();
        const detail::sgraphics& graphics() const;

        void* user_data() const { return m_user_data; }
        void user_data(void* p) { m_user_data = p; }

        void running(const bool value) { m_running = value; }
        inline bool running() const { return m_running; }

        CTX_FLAG_FUNCTION(want_resize, context_state_flag_want_resize);

        inline bool check(context_state_flag flag) const { return algorithm::bit_check(m_flags, flag); }

    private:
        core::cmutex m_mutex;
        detail::sgraphics m_graphics;
        detail::sio m_io;
        std::atomic_bool m_running = true;
        context_state_flags_t m_flags = 0;
        void* m_user_data = nullptr;
    };

} //- sm

#undef CTX_FLAG_FUNCTION
