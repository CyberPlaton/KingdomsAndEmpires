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
	void ccontext::on_begin_drawing()
	{
		m_backbuffer.bind();

		begin_default();

		//- part where rendering in world space ought to be done
		//- originally the renderpass_stack part
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::on_frame_end()
	{
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

		core::cscope_mutex m(m_mutex);

		//- Execute draw commands into default render target in a layered manner from lowest to highest
		for (const auto& pair : m_drawcommands)
		{
			for (const auto& command : pair.second)
			{
				switch (command.type())
				{
				case drawcommand_type_sprite:
				{
					const auto& sprite = command.get<drawcommand::ssprite>();

					const auto& mat = mm().get(sprite.m_material);
					const auto& tex = tm().get(sprite.m_texture);

					mat.bind();

					//- TODO: we do not use width and height from command transform, why
					src = { sprite.m_rect.x(), sprite.m_rect.y() , sprite.m_rect.w() , sprite.m_rect.h() };
					dst = { sprite.m_position.x, sprite.m_position.y, src.width, src.height };

					//- TODO: origin should be variable, probably a component thing that should be redirected to here
					raylib::DrawTexturePro(tex.texture(), src, dst, { 0.0f, 0.0f }, sprite.m_rotation,
						{ sprite.m_color.r(), sprite.m_color.g() , sprite.m_color.b() , sprite.m_color.a() });

					mat.unbind();

					break;
				}
				case drawcommand_type_rect:
				{
					const auto& rect = command.get<drawcommand::srectangle>();

					raylib::DrawRectangleV({ rect.m_position.x, rect.m_position.y }, { rect.m_size.x, rect.m_size.y }, to_cliteral(rect.m_color));
					break;
				}
				case drawcommand_type_circle:
				{
					const auto& circle = command.get<drawcommand::scircle>();

					raylib::DrawCircle(circle.m_center.x, circle.m_center.y, circle.m_radius, to_cliteral(circle.m_color));
					break;
				}
				case drawcommand_type_line:
				{
					const auto& line = command.get<drawcommand::sline>();

					raylib::DrawLineEx({ line.m_start.x, line.m_start.y }, { line.m_end.x, line.m_end.y }, line.m_thick, to_cliteral(line.m_color));
					break;
				}
				case drawcommand_type_text:
				{
					break;
				}
				case drawcommand_type_opcode:
				{
					const auto& op = command.get<drawcommand::sopcode>();

					//- execute opcode
					switch(op.m_opcode)
					{
					case drawcommand_opcode_clear:
					{
						if (op.m_data.is<core::scolor>())
						{
							const auto& color = op.m_data.get<core::scolor>();

							raylib::ClearBackground(to_cliteral(color));
						}
						else
						{
							//- warn
						}
						break;
					}
					default:
					{
						break;
					}
					}

					break;
				}
				case drawcommand_type_camera:
				{
					const auto& cam = command.get<drawcommand::scamera>();

					raylib::BeginMode2D(raylib::Camera2D{{ cam.m_position.x, cam.m_position.y }, { cam.m_offset.x, cam.m_offset.y }, cam.m_rotation, cam.m_zoom });
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
		m_drawcommands.clear();

		//- in any case we do not use camera mode for presenting backbuffer or any custom backbuffer
		raylib::EndMode2D();
		raylib::EndTextureMode();

		//- Present render target to screen
		raylib::BeginDrawing();

		if (m_has_fxaa)
		{
			const auto& technique = sm().get(m_fxaa);
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
	void ccontext::begin_default()
	{
		raylib::BeginBlendMode(raylib::BLEND_ALPHA);

		//- default clear of backbuffer
		raylib::ClearBackground(raylib::RAYWHITE);
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
		rm().get(texture).bind();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::end_render_target()
	{
		raylib::EndTextureMode();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccontext::push_commands(vector_t<cdrawcommand> buffer)
	{
		core::cscope_mutex m(m_mutex);

		for (const auto& command : buffer)
		{
			//- delegate commands to their respective layers
			switch (command.type())
			{
			case drawcommand_type_sprite:
			{
				const auto& sprite = command.get<drawcommand::ssprite>();

				m_drawcommands[sprite.m_layer].emplace_back(command);
				break;
			}
			case drawcommand_type_opcode:
			{
				const auto& op = command.get<drawcommand::sopcode>();

				m_drawcommands[op.m_layer].emplace_back(command);

				break;
			}
			case drawcommand_type_camera:
			{
				const auto& cam = command.get<drawcommand::scamera>();

				m_drawcommands[cam.m_layer].emplace_back(command);

				break;
			}
			case drawcommand_type_rect:
			{
				const auto& rect = command.get<drawcommand::srectangle>();

				m_drawcommands[rect.m_layer].emplace_back(command);

				break;
			}
			case drawcommand_type_circle:
			{
				const auto& circle = command.get<drawcommand::scircle>();

				m_drawcommands[circle.m_layer].emplace_back(command);

				break;
			}
			case drawcommand_type_line:
			{
				const auto& line = command.get<drawcommand::sline>();

				m_drawcommands[line.m_layer].emplace_back(command);

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
	ccontext::ccontext() :
		m_fxaa(invalid_handle_t), m_default(invalid_handle_t), m_sprite(invalid_handle_t), m_has_fxaa(false), m_mainwindow(nullptr)
	{
		m_spriteatlas_manager = core::cservice_manager::find<cspriteatlas_manager>();
		m_shader_manager = core::cservice_manager::find<cshader_manager>();
		m_material_manager = core::cservice_manager::find<cmaterial_manager>();
		m_texture_manager = core::cservice_manager::find<ctexture_manager>();
		m_rendertarget_manager = core::cservice_manager::find<crendertarget_manager>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccontext::~ccontext()
	{
	}

} //- sm