#include "sm_renderpass.hpp"
#include "sm_rendertarget.hpp"

namespace sm
{
    namespace renderpass
    {
        //------------------------------------------------------------------------------------------------------------------------
        bool irenderpass::init(const sconfig& cfg)
        {
            m_cfg = cfg;
            
            if(!algorithm::bit_check(m_cfg.m_flags, renderpass_flag_no_framebuffer))
            {
                m_rendertarget = core::cservice_manager::find<crendertarget_manager>()->load_sync(cfg.m_name, cfg.m_rendertarget_ratio);
            }
            else
            {
                //- We are using the default backbuffer for drawing
                m_rendertarget = C_INVALID_HANDLE;
                m_cfg.m_rendertarget_ratio = framebuffer_ratio_none;
            }
        }
    
        //------------------------------------------------------------------------------------------------------------------------
        void irenderpass::shutdown()
        {
            if(m_rendertarget != C_INVALID_HANDLE)
            {
                core::cservice_manager::find<crendertarget_manager>()->erase(m_rendertarget);
                
                m_rendertarget = C_INVALID_HANDLE;
                m_cfg.m_rendertarget_ratio = framebuffer_ratio_none;
            }
        }
        
        //------------------------------------------------------------------------------------------------------------------------
        void irenderpass::reset()
        {
            shutdown();
            init(config());
        }
    
    } //- renderpass

} //- sm
