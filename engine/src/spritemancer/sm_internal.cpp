#include "sm_internal.hpp"
#include "sm_embedded_shaders.hpp"

namespace sm
{
	namespace
	{
		constexpr mat4_t C_MATRIX_4x4_ID = mat4_t(1.0f);

		//------------------------------------------------------------------------------------------------------------------------
		raylib::Color ccolor(const core::scolor& c)
		{
			return { c.r(), c.g() ,c.b(), c.a() };
		}

	} //- unnamed

	core::resource_status ctexture_pointer::load(stringview_t name, stringview_t path)
	{
		if(ready())
		{
			return core::resource_status_ready;
		}
		else
		{
			//- TODO: let resource be loaded async and immediately return
			//- loading status, when texture is loaded set status accordingly.
// 			core::casync([&]()
// 				{
// 					m_handle = ctx().tm().create(name, path);
// 				});
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	internal::ccontext& ctx()
	{
		return internal::ccontext::instance();
	}

	namespace internal
	{
		//------------------------------------------------------------------------------------------------------------------------
		bool ccontext::init(cwindow::sconfig& cfg)
		{
			m_window = std::make_unique<cwindow>(cfg);

			//- imgui ui has to be initialized statically
			//- TODO: decide whether exclude from release builds when a gameplay UI solution
			//- is prepared

			if (cui::init())
			{
				if(m_default_rendertarget = raylib::LoadRenderTexture(cfg.m_width, cfg.m_height); raylib::IsRenderTextureReady(m_default_rendertarget))
				{
					cui::on_resize(cfg.m_width, cfg.m_height);

					//- MSAA
					if(algorithm::bit_on(cfg.m_flags, window_flag_msaa))
					{
						//- TODO: Make ps_fxaa an embedded shader
						if(m_msaa_rendertarget_technique = raylib::LoadShader(0, "resources/shaders/ps_fxaa.ps"); raylib::IsShaderReady(m_msaa_rendertarget_technique))
						{
							m_msaa_enabled = true;
						}
					}

					//- finalize
					m_screen_resolution = { cfg.m_width, cfg.m_height };
					return true;
				}
			}
			return false;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::shutdown()
		{
			m_window.reset();

			if (raylib::IsShaderReady(m_msaa_rendertarget_technique))
			{
				raylib::UnloadShader(m_msaa_rendertarget_technique);
			}
			if (raylib::IsRenderTextureReady(m_default_rendertarget))
			{
				raylib::UnloadRenderTexture(m_default_rendertarget);
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::cwindow& ccontext::window() const
		{
			ASSERT(m_window, "Invalid operation. Window was not created! Make sure to initialize context first");
			return *m_window.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::on_resize(unsigned w, unsigned h)
		{
			if (raylib::IsRenderTextureReady(m_default_rendertarget))
			{
				raylib::UnloadRenderTexture(m_default_rendertarget);
			}
			m_default_rendertarget = raylib::LoadRenderTexture(static_cast<int>(w), static_cast<int>(h));
			m_screen_resolution = { static_cast<float>(w), static_cast<float>(h) };

			sm::cui::on_resize(w, h);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::on_begin_drawing(ccamera* camera)
		{
			begin_default_render_target();
			begin_default_blend_mode();
			raylib::ClearBackground(ccolor(camera->clearcolor()));
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
			end_default_render_target();
		}

		void ccontext::on_ui_frame()
		{
			begin_default_render_target();
			cui::begin();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::on_ui_frame_end()
		{
			cui::end();
			end_default_render_target();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::on_end_drawing()
		{
			raylib::Rectangle src, dst;

			for (const auto& pair : m_drawcommands)
			{
				for (const auto& command : pair.second)
				{
					const auto& material = mm().native_resource(command.m_material);
					const auto& shader = sm().native_resource(material.get_shader());
					const auto& texture = tm().native_resource(command.m_texture);

					material.bind_dynamic_uniforms(shader);
					material.bind_shader(shader);
					material.bind_blend_mode();

					//- TODO: we do not use width and height from command transform, why
					src = {command.m_rect.x(), command.m_rect.y() , command.m_rect.w() , command.m_rect.h()};
					dst = {command.m_transform.m_x, command.m_transform.m_y, src.width, src.height};

					//- TODO: origin should be variable, probably a component thing that should be redirected to here
					DrawTexturePro(texture, src, dst, {0.0f, 0.0f}, command.m_transform.m_rotation,
						{command.m_color.r(), command.m_color.g() , command.m_color.b() , command.m_color.a()});

					material.end_blend_mode();
					material.end_shader();
				}
			}

			m_drawcommands.clear();

			raylib::BeginDrawing();

			//- check whether we should have MSAA
			if (m_msaa_enabled)
			{
				raylib::BeginShaderMode(m_msaa_rendertarget_technique);
				raylib::SetShaderValue(m_msaa_rendertarget_technique,
					raylib::GetShaderLocation(m_msaa_rendertarget_technique, "resolution"), &m_screen_resolution, raylib::SHADER_UNIFORM_VEC2);
			}

			raylib::DrawTextureRec(m_default_rendertarget.texture,
				{0.0f, 0.0f, (float)m_default_rendertarget.texture.width, (float)-m_default_rendertarget.texture.height},
				{0.0f, 0.0f}, {250, 250, 250, 255});

			if (m_msaa_enabled)
			{
				raylib::EndShaderMode();
			}

			raylib::EndDrawing();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::begin_default_render_target()
		{
			raylib::BeginTextureMode(m_default_rendertarget);
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
		void ccontext::begin_render_target(render_target_t& texture)
		{
			//- TODO
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::end_render_target(raylib::Shader combine_technique /*= {0}*/)
		{
			//- TODO
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::push_commands(vector_t<sdrawcommand>&& buffer)
		{
			//- TODO: implement the same as here but with mat2_t to not waste any space.
			//- NOTE: what to do with rotation.

// 			const glm::mat2 transform = math::translate(vec2_t{ 1.0f, 1.0f }) *
// 				math::rotate(glm::radians(45.0f)) *
// 				math::scale(vec2_t{2.0f, 1.0f});
// 
// 			const auto p = math::decompose_translation(transform);
// 			const auto s = math::decompose_scale(transform);

			vec3_t position, scale, rotation;

			for (const auto& command : buffer)
			{
				const mat4_t transform = glm::translate(C_MATRIX_4x4_ID, vec3_t(command.m_position, 0.0f)) *
					glm::rotate(C_MATRIX_4x4_ID, glm::radians(command.m_rotation), vec3_t(0.0f, 0.0f, 1.0f)) *
					glm::scale(C_MATRIX_4x4_ID, vec3_t(command.m_dimension, 1.0f));

				math::decompose_translation(transform, position);
				math::decompose_rotation(transform, rotation);
				math::decompose_scale(transform, scale);

				m_drawcommands[command.m_layer].emplace_back(command.m_rect,
					sdecomposed_transform{position.x, position.y, scale.x, scale.y, rotation.z},
					command.m_color, command.m_material, command.m_texture);
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		ccontext::ccontext() :
			m_spriteatlas_manager(*this),
			m_technique_manager(*this),
			m_material_manager(*this),
			m_texture_manager(*this)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		ccontext::~ccontext()
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccontext::end_default_render_target()
		{
			raylib::EndTextureMode();
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ctechnique_manager::init()
		{
			if (const auto technique = create_embedded("sprite", programs::pixelperfect::s_vs, programs::pixelperfect::s_ps);
				algorithm::is_valid_handle(technique) &&
				ctx().mm().create_default(technique, blending_mode_alpha, blending_equation_blend_color,
					blending_factor_src_color, blending_factor_one_minus_src_alpha))
			{
				return true;
			}
			return false;
		}

		//------------------------------------------------------------------------------------------------------------------------
		spriteatlas_t cspriteatlas_manager::create(stringview_t spriteatlasname, stringview_t texturepath, const vec2_t& frames)
		{
			spriteatlas_t handle = invalid_handle_t;

			auto construct_atlas = [&](texture_t texture, csprite_atlas& atlas)
				{
					const auto texturesize = ctx().tm().texture_size(texture);
					const vec2_t framesize = { texturesize.x / frames.x, texturesize.y / frames.y };

					atlas.begin(texture);

					for (auto y = 0u; y < frames.y; ++y)
					{
						for (auto x = 0u; x < frames.x; ++x)
						{
							atlas.subtexture({ x * framesize.x / texturesize.x,
								y * framesize.y / texturesize.y,
								framesize.x / texturesize.x,
								framesize.y / texturesize.y });
						}
					}

					atlas.end();
				};


			if (auto texture = ctx().tm().create(fmt::format("{}_texture", spriteatlasname).c_str(), texturepath);
				algorithm::is_valid_handle(texture))
			{
				if (fragmented())
				{
					if (const auto slot = fragmentation_slot(); slot != std::numeric_limits<unsigned>().max())
					{
						//- manually construct a sprite atlas
						auto& atlas = m_resources[slot].m_resource;

						construct_atlas(texture, atlas);

						m_resources[slot].m_removed = false;
						decrement();
						handle = slot;
					}
				}
				else
				{
					const auto h = algorithm::hash(spriteatlasname);

					handle = increment();
					m_lookup.emplace(h, handle);
					auto& wrapper = m_resources.emplace_back();

					construct_atlas(texture, wrapper.m_resource);
				}
			}
			return handle;
		}

		//------------------------------------------------------------------------------------------------------------------------
		irenderer::~irenderer()
		{
			submit();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void irenderer::draw_sprite(renderlayer_t layer, const vec2_t& position, float rotation, const vec2_t& scale,
			texture_t texture, material_t material, const core::srect& rect, const core::scolor& color, bool flipx, bool flipy)
		{
			auto __rect = rect;
			if(flipx) __rect.m_w = (-__rect.m_w);
			if (flipy) __rect.m_h = (-__rect.m_h);

			m_buffer.push(__rect, color, position, scale, rotation, material, texture, layer);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void irenderer::draw_spriteatlas_sprite(renderlayer_t layer, const vec2_t& position, float rotation, const vec2_t& scale,
			spriteatlas_t atlas, subtexture_t subtexture, const core::scolor& color, bool flipx, bool flipy)
		{
			const auto& __atlas = ctx().am().native_resource(atlas);

			draw_sprite(layer, position, rotation, scale, __atlas.atlas(), C_DEFAULT_MATERIAL,
				__atlas.subtexture(subtexture), color, flipx, flipy);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void irenderer::submit()
		{
			ctx().push_commands(m_buffer.take());
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

	} //- internal

} //- sm