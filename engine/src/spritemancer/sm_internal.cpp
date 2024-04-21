#include "sm_internal.hpp"
#include "sm_embedded_shaders.hpp"

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		inline raylib::Color to_cliteral(const core::scolor& c)
		{
			return { c.r(), c.g() ,c.b(), c.a() };
		}

	} //- unnamed

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		irenderer::~irenderer()
		{
			submit();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void irenderer::submit()
		{
			ctx().push_commands(std::move(m_commands.take()));
		}

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	ccontext& ctx()
	{
		return ccontext::instance();
	}

	//- Responsible for creating main window and OpenGL context,
	//- initialzing ImGui ui (when required), and loading default or basic rendering related resources.
	//------------------------------------------------------------------------------------------------------------------------
	bool ccontext::init(cwindow::sconfig& cfg)
	{
		m_mainwindow = std::make_unique<cwindow>(cfg);

		//- imgui ui has to be initialized statically
		//- TODO: decide whether exclude from release builds when a gameplay UI solution is prepared
		if (!cui::init())
		{
			//- report failure, depending on whether we expect using ImGui
			return false;
		}

		cui::on_resize(cfg.m_width, cfg.m_height);

		//- default framebuffer
		if (!m_backbuffer.create(cfg.m_width, cfg.m_height))
		{
			//- report failure
			return false;
		}

		//- default sprite shader
		if (m_sprite = sm().load_from_memory(C_SPRITE_TECHNIQUE_NAME.data(), programs::pixelperfect::s_vs, programs::pixelperfect::s_ps);
			!algorithm::is_valid_handle(m_sprite))
		{
			//- report failure
			return false;
		}

		//- default material
		if (m_default = mm().load(C_DEFAULT_MATERIAL_NAME.data(), m_sprite, blending_mode_alpha, blending_equation_blend_color,
			blending_factor_src_color, blending_factor_one_minus_src_alpha);
			!algorithm::is_valid_handle(m_default))
		{
			//- report failure
			return false;
		}

		//- anti-aliasing shader
		if (algorithm::bit_on(cfg.m_flags, window_flag_fxaa))
		{
			m_fxaa = sm().load_from_memory(C_FXAA_TECHNIQUE_NAME.data(), 0, programs::fxaa::s_ps);

			if (!algorithm::is_valid_handle(m_fxaa))
			{
				//- could not load fxaa shader, report a warning and disable it
				m_has_fxaa = false;
			}
			m_has_fxaa = true;
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::shutdown()
	{
		m_mainwindow.reset();
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cwindow& ccontext::window() const
	{
		ASSERT(m_mainwindow, "Invalid operation. Window was not created! Make sure to initialize context first");
		return *m_mainwindow.get();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::on_resize(unsigned w, unsigned h)
	{
		m_backbuffer.create(w, h);

		cui::on_resize(w, h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::on_begin_drawing(ccamera* camera)
	{
		m_backbuffer.bind();
		begin_default_blend_mode();
		raylib::ClearBackground(to_cliteral(camera->clearcolor()));
		camera->use();
		m_frame_camera = camera;

		//- part where rendering in world space ought to be done
		//- originally the renderpass_stack part
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::on_frame_end()
	{
		m_frame_camera->end();
		end_blend_mode();
		m_backbuffer.unbind();
	}

	void ccontext::on_ui_frame()
	{
		m_backbuffer.bind();
		cui::begin();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::on_ui_frame_end()
	{
		cui::end();
		m_backbuffer.unbind();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::on_end_drawing()
	{
		raylib::Rectangle src, dst;

		//- Execute draw commands into default render target in a layered manner from lowest to highest
		for (const auto& pair : m_drawcommands)
		{
			for (const auto& command : pair.second)
			{
				const auto& mat = mm().get(command.m_material);
				const auto& tex = tm().get(command.m_texture);

				mat.bind();

				//- TODO: we do not use width and height from command transform, why
				src = { command.m_rect.x(), command.m_rect.y() , command.m_rect.w() , command.m_rect.h() };
				dst = { command.m_transform.m_x, command.m_transform.m_y, src.width, src.height };

				//- TODO: origin should be variable, probably a component thing that should be redirected to here
				DrawTexturePro(tex.texture(), src, dst, {0.0f, 0.0f}, command.m_transform.m_rotation,
					{ command.m_color.r(), command.m_color.g() , command.m_color.b() , command.m_color.a() });

				mat.unbind();
			}
		}
		m_drawcommands.clear();

		//- Present render target to screen
		raylib::BeginDrawing();

		if (m_has_fxaa)
		{
			const auto& technique = m_shader_manager.get(m_fxaa);
			const auto& rendertarget = m_backbuffer.target();
			const auto resolution = vec2_t(rendertarget.texture.width, rendertarget.texture.height);

			raylib::BeginShaderMode(technique.shader());

			raylib::SetShaderValue(technique.shader(), raylib::GetShaderLocation(technique.shader(), C_FXAA_TECHNIQUE_RESOLUTION_PROP.data()),
				&resolution, raylib::SHADER_UNIFORM_VEC2);
		}

		const auto w = SCAST(float, m_backbuffer.target().texture.width);
		const auto h = SCAST(float, m_backbuffer.target().texture.height);

		raylib::DrawTextureRec(m_backbuffer.target().texture, { 0.0f, 0.0f, w, -h }, { 0.0f, 0.0f }, to_cliteral(C_COLOR_WHITE));

		if (m_has_fxaa)
		{
			raylib::EndShaderMode();
		}

		raylib::EndDrawing();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::begin_default_blend_mode()
	{
		raylib::BeginBlendMode(raylib::BLEND_ALPHA);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::begin_blend_mode(blending_mode mode)
	{
		raylib::BeginBlendMode(mode);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::end_blend_mode()
	{
		raylib::EndBlendMode();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::begin_render_target(rendertarget_t texture)
	{
		m_rendertarget_manager.get(texture).bind();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::end_render_target()
	{
		raylib::EndTextureMode();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::push_commands(vector_t<cdrawcommand>&& buffer)
	{
		for (const auto& command : buffer)
		{
			switch (command.type())
			{
			case drawcommand_type_sprite:
			{
				const auto& sprite = command.get<drawcommand::ssprite>();

				//- perform affine transformation and store for later rendering
				//- TODO: how to handle parent-child relationships
				const auto [p, s, r] = math::transform(sprite.m_position, sprite.m_dimension, { 0.0f, 0.0f },
					sprite.m_rotation);

					m_drawcommands[sprite.m_layer].emplace_back(sprite.m_rect,
						sdecomposed_transform{ p.x, p.y, s.x, s.y, r },
						sprite.m_color, algorithm::is_valid_handle(sprite.m_material) ? sprite.m_material : m_default,
						sprite.m_texture);
			}
			case drawcommand_type_rect:
			{
				break;
			}
			case drawcommand_type_circle:
			{
				break;
			}
			case drawcommand_type_line:
			{
				break;
			}
			case drawcommand_type_text:
			{
				break;
			}
			default:
			{
				//- report unknown command type
				break;
			}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccontext::ccontext()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccontext::~ccontext()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccontext::ssprite_drawcommand::ssprite_drawcommand(core::srect rect, ccontext::sdecomposed_transform transform, core::scolor color,
		material_t material, texture_t texture) :
		m_rect(std::move(rect)), m_transform(std::move(transform)), m_color(std::move(color)), m_material(material), m_texture(texture)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccontext::sdecomposed_transform::sdecomposed_transform(float x, float y, float w, float h, float r) :
		m_x(x), m_y(y), m_w(w), m_h(h), m_rotation(r)
	{
	}

} //- sm