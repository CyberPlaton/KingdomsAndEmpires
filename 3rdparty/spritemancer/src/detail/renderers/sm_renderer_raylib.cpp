#include "sm_renderer_raylib.hpp"
#include "../sm_embedded_shaders.hpp"
#include "../sm_rendering_layer.hpp"

namespace sm
{
	namespace shaders
	{
		constexpr stringview_t C_VERTEX_DEFAULT =
			"#version 330\n"
			"in vec3 vertPosition;\n"
			"in vec2 vertTexCoord;\n"
			"in vec4 vertColor;\n"
			"uniform mat4 mvp;\n"
			"uniform mat4 matModel;\n"
			"out vec3 fragPosition;\n"
			"out vec2 fragTexCoord;\n"
			"out vec4 fragColor;\n"
			"void main()\n"
			"{\n"
			"fragPosition = vec3(matModel*vec4(vertPosition, 1.0));\n"
			"fragTexCoord = vertTexCoord;\n"
			"fragColor = vertColor;\n"
			"gl_Position = mvp*vec4(vertPosition, 1.0);\n"
			"}\n"
			;

		constexpr stringview_t C_FRAGMENT_DEFAULT =
			"#version 330\n"
			"in vec3 fragPosition;\n"
			"in vec2 fragTexCoord;\n"
			"in vec4 fragColor;\n"
			"uniform sampler2D texture0;\n"
			"uniform vec4 colDiffuse;\n"
			"vec2 uv_klems( vec2 uv, ivec2 texture_size )\n"
			"{\n"
			"vec2 pixels = uv * texture_size + 0.5;\n"
			"vec2 fl = floor(pixels);\n"
			"vec2 fr = fract(pixels);\n"
			"vec2 aa = fwidth(pixels) * 0.75;\n"
			"fr = smoothstep( vec2(0.5, 0.5) - aa, vec2(0.5, 0.5) + aa, fr);\n"
			"return (fl + fr - 0.5) / texture_size;\n"
			"}\n"
			"void main()\n"
			"{\n"
			"vec2 texcoord = uv_klems(fragTexCoord, textureSize(texture0, 0));\n"
			"vec4 color = colDiffuse * texture2D(texture0, texcoord);\n"
			"if (color.w < 1.0/255.0)\n"
			"{\n"
			"discard;\n"
			"}\n"
			"gl_FragColor = color;\n"
			"}\n"
			;

	} //- shaders

	namespace
	{
		static uint16_t S_W = 0;
		static uint16_t S_H = 0;
		static uint16_t S_X = 0;
		static uint16_t S_Y = 0;
		static core::scolor S_RT_CLEAR_COLOR = { 50, 50, 155, 255 };
		static core::scolor S_FBO_CLEAR_COLOR = { 155, 50, 50, 255 };
		static raylib::Camera2D S_FRAME_CAMERA;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::prepare_device()
	{
		//- noop
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crenderer_raylib::init_device(void* nwh, unsigned w, unsigned h, bool fullscreen, bool vsync)
	{
		if (serror_reporter::instance().m_callback)
		{
			serror_reporter::instance().m_callback(core::logging_verbosity_info, "Initializing raylib...");
		}

		S_W = w;
		S_H = h;

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crenderer_raylib::shutdown_device()
	{
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::update_viewport(const vec2_t& position, const vec2_t& size)
	{
		S_X = (uint16_t)/*position.x*/0;
		S_Y = (uint16_t)/*position.y*/0;
		S_W = (uint16_t)size.x;
		S_H = (uint16_t)size.y;

		//- FIXME: we set the camera to be centralized on target, while this probably should not be our responsibility, but that
		//- of the camera system.
		S_FRAME_CAMERA.offset = { S_W * 0.5f, S_H * 0.5f };
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::blendmode(sblending mode)
	{
		raylib::rlSetBlendMode(mode.m_mode);
		raylib::rlSetBlendFactors(mode.m_src_factor, mode.m_dst_factor, mode.m_equation);
		raylib::rlSetBlendMode(raylib::BLEND_CUSTOM);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::blendmode_end()
	{
		raylib::EndBlendMode();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::update_frame_camera(const ccamera& camera)
	{
		S_FRAME_CAMERA = camera.camera();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::state_reset_to_default()
	{
		raylib::EndMode2D();
		raylib::EndShaderMode();
		raylib::EndBlendMode();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::begin_main_render_texture(const crendertarget& target)
	{
		raylib::BeginTextureMode(target.target());
		raylib::BeginMode2D(S_FRAME_CAMERA);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::end_main_render_texture(const crendertarget& /*target*/)
	{
		raylib::EndTextureMode();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::clear_main_render_texture(const crendertarget& target, bool depth)
	{
		raylib::ClearBackground(to_cliteral(S_RT_CLEAR_COLOR));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::draw_main_render_texture(const crendertarget& target)
	{
		const auto w = (float)target.target().texture.width;
		const auto h = (float)target.target().texture.height;

		raylib::Rectangle src = { 0.0f, 0.0f, w, -h };
		raylib::Rectangle dst = { 0.0f, 0.0f, w, h };

		raylib::DrawTexturePro(target.target().texture, src, dst, { 0.0f, 0.0f }, 0.0f, { 255, 255, 255, 255 });
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::begin_default_backbuffer_drawing()
	{
		raylib::BeginDrawing();
		raylib::ClearBackground(to_cliteral(S_FBO_CLEAR_COLOR));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::end_default_backbuffer_drawing()
	{
		raylib::EndDrawing();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::layer_draw(const srendering_layer& layer)
	{
		for (const auto& command : layer.m_commands)
		{
			command.execute();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool crenderer_raylib::imgui_begin()
	{
		imgui::begin();

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::imgui_end()
	{
		imgui::end();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::update_texture_gpu(uint64_t id, unsigned w, unsigned h, texture_format format, const void* data)
	{
		raylib::rlUpdateTexture(id, 0, 0, w, h, format, data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::update_texture_cpu(uint64_t id, unsigned w, unsigned h, texture_format format, void*& data)
	{
		if (void* _data = raylib::rlReadTexturePixels(id, w, h, format); _data)
		{
			data = _data;
		}
	}

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	rttr::cregistrator<crenderer_raylib>("crenderer_raylib")
		;
}
