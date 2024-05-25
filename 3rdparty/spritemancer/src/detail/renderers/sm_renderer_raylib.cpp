#include "sm_renderer_raylib.hpp"
#include "../sm_embedded_shaders.hpp"

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
		static core::scolor S_WHITE = { 255, 255, 255, 255 };
		static cimage S_IMAGE_BLANK;
		static ctexture S_TEXTURE_BLANK;
		static raylib::Camera2D S_CAMERA = {{0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 0.0f};

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::prepare_device()
	{
		//- empty
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

		S_IMAGE_BLANK.create_solid(1, 1, S_WHITE);
		S_TEXTURE_BLANK.load_from_image(S_IMAGE_BLANK);
		S_CAMERA.offset = { S_W / 2.0f, S_H / 2.0f };

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crenderer_raylib::shutdown_device()
	{
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::prepare_frame()
	{
		raylib::BeginDrawing();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::display_frame()
	{
		raylib::DrawRectangle(0, 0, S_W, 5, raylib::RED);
		raylib::DrawRectangle(0, 5, 5, S_H - 10, raylib::RED);
		raylib::DrawRectangle(S_W - 5, 5, 5, S_H - 10, raylib::RED);
		raylib::DrawRectangle(0, S_H - 5, S_W, 5, raylib::RED);

		raylib::EndDrawing();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::update_viewport(const vec2_t& position, const vec2_t& size)
	{
		S_X = (uint16_t)/*position.x*/0;
		S_Y = (uint16_t)/*position.y*/0;
		S_W = (uint16_t)size.x;
		S_H = (uint16_t)size.y;

		S_CAMERA.offset = { S_W / 2.0f, S_H / 2.0f };
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::clear(unsigned view_id, const core::scolor& color, bool depth)
	{
		raylib::ClearBackground(to_cliteral(color));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::blendmode(sblending mode)
	{
		raylib::rlSetBlendMode(mode.m_mode);
		raylib::rlSetBlendFactors(mode.m_src_factor, mode.m_dst_factor, mode.m_equation);
		raylib::rlSetBlendMode(raylib::BLEND_CUSTOM);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::bind_texture(uint64_t id)
	{
		raylib::rlEnableTexture(static_cast<unsigned>(id));
	}

	//- This is basically clearing a layer with a color. Unsure whether we require this...
	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::render_layer_quad(const vec2_t& position, const vec2_t& size, const core::scolor& color)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::render_decal(const sdecal& decal)
	{
		using namespace raylib;

		blendmode(decal.m_blending);

		const auto valid_texture = decal.m_texture > 0;

		//- either use blank quad or decals texture for drawing
		if (valid_texture)
		{
			rlEnableTexture(decal.m_texture);
		}
		else
		{
			rlEnableTexture(S_TEXTURE_BLANK.texture().id);
		}

		//- either draw in screen space or in world space
		if (decal.m_camera_mode == camera_mode_2d)
		{
			BeginMode2D(S_CAMERA);
		}

		rlBegin(RL_QUADS);

		if (valid_texture)
		{
			for (auto i = 0u; i < decal.m_vertices.size(); ++i)
			{
				rlColor4ub(decal.m_tints[i].r(),
					decal.m_tints[i].g(),
					decal.m_tints[i].b(),
					decal.m_tints[i].a());

				rlNormal3f(0.0f, 0.0f, 1.0f);

				//- set texture UV coordinate for vertex
				rlTexCoord2f(decal.m_uvs[i].x, decal.m_uvs[i].y);

				//- set vertex position
				rlVertex2f(decal.m_vertices[i].x, decal.m_vertices[i].y);
			}
		}
		else
		{
			for (auto i = 0u; i < decal.m_vertices.size(); ++i)
			{
				rlColor4ub(decal.m_tints[i].r(),
					decal.m_tints[i].g(),
					decal.m_tints[i].b(),
					decal.m_tints[i].a());

				rlNormal3f(0.0f, 0.0f, 1.0f);

				//- set vertex position
				rlVertex2f(decal.m_vertices[i].x, decal.m_vertices[i].y);
			}
		}

		rlEnd();
		rlSetTexture(0);

		if (decal.m_camera_mode == camera_mode_2d)
		{
			EndMode2D();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::update_texture_gpu(uint64_t id, unsigned w, unsigned h, void* data)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer_raylib::update_texture_cpu(uint64_t id)
	{

	}

} //- sm