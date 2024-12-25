#pragma once
#include "sm_config.hpp"

namespace sm
{
    namespace renderpass
    {
        using renderpass_flags_t = int;
        
        //------------------------------------------------------------------------------------------------------------------------
        enum renderpass_flag : uint16_t
        {
            renderpass_flag_no_framebuffer = BIT(0),
        };
        
        //------------------------------------------------------------------------------------------------------------------------
        struct sconfig final
        {
            string_t m_name;
            view_mode m_view_mode = view_mode_none;
            framebuffer_ratio m_rendertarget_ratio = framebuffer_ratio_none;
            renderpass_flags_t m_flags = 0;
        };
    
        //- A renderpass interface. A renderpass defines a view that can accept draw calls from the application. It can have a
        //- render texture into which we render or not. It defines its order in the applications passes and can set additional settings.
        //- Architecturally this is a mix of engines system struct and component struct concepts:
        //- Define your renderpass by extending this class and reflecting to RTTR using DECLARE_RENDERPASS(...)
        //- and REGISTER_RENDERPASS(...), and implementing relevant static and/or virtual functions.
        //------------------------------------------------------------------------------------------------------------------------
        struct irenderpass
        {
            virtual sconfig config() { return {}; }
            virtual bool init(const sconfig& cfg);
            virtual void shutdown();
            virtual void reset();
            
            sconfig m_cfg;
            renderpass_id_t m_id = C_INVALID_HANDLE; //- Id serves as identifier for renderpass and the view index
            rendertarget_handle_t m_rendertarget = C_INVALID_HANDLE;
            
            RTTR_ENABLE();
        };
    
    } //- renderpass

    using renderpass_ref_t = ref_t<renderpass::irenderpass>;

} //- sm
