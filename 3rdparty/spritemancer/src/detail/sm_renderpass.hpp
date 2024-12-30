#pragma once
#include "sm_config.hpp"

#define DECLARE_RENDERPASS(c) \
c() { m_cfg = c::config(); }; \
~c() = default; \
static stringview_t name() { static constexpr stringview_t C_NAME = STRING(c); return C_NAME; } \
static renderpass_ref_t create() { return std::make_shared<c>(); }

namespace sm
{
    namespace renderpass
    {
        using renderpass_flags_t = int;
        
        //------------------------------------------------------------------------------------------------------------------------
        enum renderpass_flag : uint16_t
        {
            renderpass_flag_no_framebuffer = BIT(0),
			renderpass_flag_no_program = BIT(1),
        };

        //------------------------------------------------------------------------------------------------------------------------
        struct sconfig final
        {
			renderpass_id_t m_id					= C_INVALID_HANDLE; //- Id serves as identifier for renderpass and the view index
            view_mode m_view_mode					= view_mode_none;
            framebuffer_ratio m_rendertarget_ratio	= framebuffer_ratio_none;
			rendertarget_handle_t m_rendertarget	= C_INVALID_HANDLE;
			program_handle_t m_program				= C_INVALID_HANDLE;
            renderpass_flags_t m_flags				= 0;
        };

        //- A renderpass interface. A renderpass defines a view that can accept draw calls from the application. It can have a
        //- render texture into which we render or not. It defines its order in the applications passes and can set additional settings.
        //- Architecturally this is a mix of engines system struct and component struct concepts:
        //- Define your renderpass by extending this class and reflecting to RTTR using DECLARE_RENDERPASS(...)
        //- and REGISTER_RENDERPASS(...), and implementing relevant static and/or virtual functions.
        //------------------------------------------------------------------------------------------------------------------------
		struct irenderpass
        {
			static constexpr rttr::string_view C_RENDERPASS_CONFIG_FUNC_NAME = "config";
			static constexpr rttr::string_view C_RENDERPASS_CREATE_FUNC_NAME = "create";
			static constexpr array_t<rttr::string_view, 2> C_RENDERPASS_FUNC_NAMES = { C_RENDERPASS_CONFIG_FUNC_NAME,
				C_RENDERPASS_CREATE_FUNC_NAME };

			static stringview_t name() { static constexpr stringview_t C_NAME = "irenderpass"; return C_NAME; };
			static sconfig config() { return {}; }
			renderpass_ref_t create() { return std::make_shared<irenderpass>(); }

			//- Current configuration for this renderpass. Can be altered at runtime.
			sconfig m_cfg;

			//- Current camera data set for renderpass
			mat4_t m_view_mtx = math::C_MAT4_ID;
			mat4_t m_projection_mtx = math::C_MAT4_ID;

            RTTR_ENABLE();
        };

		//------------------------------------------------------------------------------------------------------------------------
		struct smain_renderpass final : irenderpass
		{
			DECLARE_RENDERPASS(smain_renderpass);

			static sconfig config();

			RTTR_ENABLE(irenderpass);
		};

    } //- renderpass

} //- sm
