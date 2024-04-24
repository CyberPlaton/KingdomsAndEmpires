#include "sm_common.hpp"

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		void set_window_state(raylib::ConfigFlags flag, bool value)
		{
			value? raylib::SetWindowState(flag) : raylib::ClearWindowState(flag);
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	sm::cdrawcommand cdrawcommand::camera(const vec2_t& position, const vec2_t& offset, float zoom, float rotation,
		const core::scolor& clearcolor, renderlayer_t layer)
	{
		return cdrawcommand(drawcommand_type_camera, drawcommand::scamera{ clearcolor, position, offset, rotation, zoom, layer });
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cdrawcommand cdrawcommand::opcode(drawcommand_opcode code, core::cany data, renderlayer_t layer)
	{
		return cdrawcommand(drawcommand_type_opcode, drawcommand::sopcode{ code, data, layer });
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cdrawcommand cdrawcommand::sprite(const core::srect& rect, const core::scolor& color, float x, float y, float w, float h,
		float r, material_t material, texture_t texture, renderlayer_t layer)
	{
		return sprite(rect, color, {x, y}, {w, h}, r, material ,texture, layer);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cdrawcommand cdrawcommand::sprite(const core::srect& rect, const core::scolor& color, const vec2_t& position,
		const vec2_t& dimension, float r, material_t material, texture_t texture, renderlayer_t layer)
	{
		return cdrawcommand(drawcommand_type_sprite, drawcommand::ssprite{rect, color, position, dimension, r, material, texture, layer});
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cdrawcommand cdrawcommand::line(const vec2_t& start, const vec2_t& end, float thick,
		const core::scolor& color, renderlayer_t layer)
	{
		return cdrawcommand(drawcommand_type_line, drawcommand::sline{ color, start, end, thick, layer });
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cdrawcommand cdrawcommand::circle(const vec2_t& center, float radius, const core::scolor& color, renderlayer_t layer)
	{
		return cdrawcommand(drawcommand_type_circle, drawcommand::scircle{ color, center, radius, layer});
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cdrawcommand cdrawcommand::rectangle(const vec2_t& position, const vec2_t& dimension,
		const core::scolor& color, renderlayer_t layer)
	{
		return cdrawcommand(drawcommand_type_rect, drawcommand::srectangle{ color, position, dimension, layer});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccommandbuffer::push(cdrawcommand command)
	{
		m_commands.emplace_back(std::move(command));
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture(const cimage& image) :
		m_texture(raylib::LoadTextureFromImage(image.image()))
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture(const raylib::Texture& texture) :
		m_texture(texture)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture() :
		m_texture({ 0 })
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::~ctexture()
	{
		if (ready())
		{
			raylib::UnloadTexture(texture());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned ctexture::width() const
	{
		return m_texture.width;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned ctexture::height() const
	{
		return m_texture.height;
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::crendertarget(unsigned w, unsigned h) :
		m_image(raylib::LoadRenderTexture(w, h))
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::crendertarget() :
		m_image({ 0 })
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::~crendertarget()
	{
		if (ready())
		{
			raylib::UnloadRenderTexture(target());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool crendertarget::create(unsigned w, unsigned h)
	{
		if (ready())
		{
			raylib::UnloadRenderTexture(target());
		}

		m_image = raylib::LoadRenderTexture(w, h);

		return ready();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crendertarget::bind() const
	{
		if (ready())
		{
			raylib::BeginTextureMode(target());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crendertarget::unbind() const
	{
		raylib::EndTextureMode();
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctechnique::ctechnique(const raylib::Shader& shader) :
		m_shader(shader)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctechnique::ctechnique() :
		m_shader({ 0 })
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctechnique::~ctechnique()
	{
		if (raylib::IsShaderReady(m_shader))
		{
			raylib::UnloadShader(m_shader);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctechnique::bind() const
	{
		if (raylib::IsShaderReady(m_shader))
		{
			raylib::BeginShaderMode(m_shader);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctechnique::unbind() const
	{
		raylib::EndShaderMode();
	}

	//------------------------------------------------------------------------------------------------------------------------
	int ctechnique::uniform_location(stringview_t name) const
	{
		return raylib::GetShaderLocation(shader(), name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cwindow::~cwindow()
	{
		raylib::ShutdownWindow();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cwindow::enter_fullscreen()
	{
		if (!fullscreen())
		{
			raylib::ToggleFullscreen();
			raylib::MaximizeWindow();
			raylib::ToggleBorderlessWindowed();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cwindow::exit_fullscreen()
	{
		if (fullscreen())
		{
			raylib::ToggleFullscreen();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cwindow::fullscreen() const
	{
		return raylib::IsWindowFullscreen();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cwindow::show()
	{
		raylib::ClearWindowState(raylib::FLAG_WINDOW_HIDDEN);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cwindow::hide()
	{
		raylib::SetWindowState(raylib::FLAG_WINDOW_HIDDEN);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cwindow::resize(unsigned w, unsigned h)
	{
		raylib::SetWindowSize(SCAST(int, w), SCAST(int, h));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cwindow::position(unsigned w, unsigned h)
	{
		raylib::SetWindowPosition(SCAST(int, w), SCAST(int, h));
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cwindow::x() const
	{
		return SCAST(unsigned, raylib::GetWindowPosition().x);
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cwindow::y() const
	{
		return SCAST(unsigned, raylib::GetWindowPosition().y);
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cwindow::w() const
	{
		return SCAST(unsigned, raylib::GetScreenWidth());
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cwindow::h() const
	{
		return SCAST(unsigned, raylib::GetScreenHeight());
	}

	//------------------------------------------------------------------------------------------------------------------------
	ivec2_t cwindow::dimension_for_resolution(window_resolution resolution)
	{
		constexpr array_t<ivec2_t, 11> C_DIMENSIONS =
		{
			{{0, 0},
			{1280, 720},
			{1280, 800},
			{1280, 1024},
			{1366, 768},
			{1600, 900},
			{1920, 1080},
			{1920, 1200},
			{2560, 1440},
			{3440, 1440},
			{3840, 2160},}
		};
		return C_DIMENSIONS[(uint8_t)resolution];
	}

	//------------------------------------------------------------------------------------------------------------------------
	ivec2_t cwindow::monitor_dimension()
	{
		static const auto monitor = raylib::GetCurrentMonitor();
		return { SCAST(unsigned, raylib::GetMonitorWidth(monitor)), SCAST(unsigned, raylib::GetMonitorHeight(monitor)) };
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cwindow::monitor_refresh_rate()
	{
		static const auto monitor = raylib::GetCurrentMonitor();
		return SCAST(unsigned, raylib::GetMonitorRefreshRate(monitor));
	}

	//------------------------------------------------------------------------------------------------------------------------
	cwindow::cwindow(sconfig& cfg)
	{
		using namespace algorithm;

		//- basic init of window
		if (bit_on(cfg.m_flags, window_flag_vsync))
		{
			raylib::SetWindowState(raylib::FLAG_VSYNC_HINT);
		}
		if (bit_on(cfg.m_flags, window_flag_msaa))
		{
			raylib::SetWindowState(raylib::FLAG_MSAA_4X_HINT);
		}

		int w = cfg.m_width, h = cfg.m_height;

		if (cfg.m_resolution != window_resolution_custom)
		{
			auto size = dimension_for_resolution(cfg.m_resolution);

			w = size.x; h = size.y;
		}

		raylib::InitWindow(w, h, cfg.m_title.c_str());

		hide();

		//- icon
		if (bit_on(cfg.m_flags, window_flag_has_icon))
		{
			if (const auto icon = raylib::LoadImage(cfg.m_window_icon_path.c_str()); raylib::IsImageReady(icon))
			{
				raylib::SetWindowIcon(icon);
			}
		}

		//- target fps
		int target = raylib::GetMonitorRefreshRate(raylib::GetCurrentMonitor());
		if (cfg.m_target_fps != 0)
		{
			target = cfg.m_target_fps;
		}

		raylib::SetTargetFPS(target);

		//- misc
		set_window_state(raylib::FLAG_WINDOW_UNDECORATED, !bit_on(cfg.m_flags, window_flag_decorated));
		set_window_state(raylib::FLAG_WINDOW_MINIMIZED, bit_on(cfg.m_flags, window_flag_minimized));
		set_window_state(raylib::FLAG_WINDOW_RESIZABLE, bit_on(cfg.m_flags, window_flag_resizable));
		set_window_state(raylib::FLAG_WINDOW_ALWAYS_RUN, bit_on(cfg.m_flags, window_flag_run_minimized));
		set_window_state(raylib::FLAG_WINDOW_UNFOCUSED, !bit_on(cfg.m_flags, window_flag_focus));

		//- fullscreen
		if (bit_on(cfg.m_flags, window_flag_fullscreen))
		{
			enter_fullscreen();
		}

		//- finalize
		if (bit_on(cfg.m_flags, window_flag_show))
		{
			show();
		}

		cfg.m_width = w;
		cfg.m_height = h;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::srect ccamera::viewport() const
	{
		return m_viewport;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccamera::viewport(float x, float y, float w, float h)
	{
		m_viewport.set(x, y, w, h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::scolor ccamera::clearcolor() const
	{
		return m_clearcolor;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccamera::clearcolor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		m_clearcolor.m_r = r;
		m_clearcolor.m_g = g;
		m_clearcolor.m_b = b;
		m_clearcolor.m_a = a;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccamera::move_to(vec2_t value)
	{
		m_camera.target.x = value.x;
		m_camera.target.y = value.y;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccamera::move(vec2_t value)
	{
		m_camera.target.x += value.x;
		m_camera.target.y += value.y;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccamera::rotate_to(float value)
	{
		m_camera.rotation = value;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccamera::rotate(float value)
	{
		m_camera.rotation += value;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccamera::zoom_to(float value)
	{
		m_camera.zoom = value;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccamera::zoom(float value)
	{
		m_camera.zoom += value;
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t ccamera::position() const
	{
		return { m_camera.target.x, m_camera.target.y };
	}

	//------------------------------------------------------------------------------------------------------------------------
	float ccamera::rotation() const
	{
		return m_camera.rotation;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float ccamera::zoom() const
	{
		return m_camera.zoom;
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t ccamera::vector_screen_to_world(const vec2_t& point)
	{
		auto v = raylib::GetScreenToWorld2D({ point.x, point.y }, m_camera);
		return { v.x, v.y };
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t ccamera::vector_world_to_screen(const vec2_t& point)
	{
		auto v = raylib::GetWorldToScreen2D({ point.x, point.y }, m_camera);
		return { v.x, v.y };
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::srect ccamera::world_visible_area() const
	{
		auto z = m_camera.zoom;
		auto target = m_camera.target;
		auto offset = m_camera.offset;

		return { target.x - (1.0f / z) * offset.x + 5.0f,
				 target.y - (1.0f / z) * offset.y + 5.0f,
				 (1.0f / z) * offset.x * 2.0f - 5.0f,
				 (1.0f / z) * offset.y * 2.0f - 5.0f };
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccamera::use()
	{
		raylib::BeginMode2D(m_camera);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccamera::end()
	{
		raylib::EndMode2D();
	}

	//------------------------------------------------------------------------------------------------------------------------
	raylib::Camera2D ccamera::get_camera() const
	{
		return m_camera;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cspriteatlas::reset()
	{
		m_texture = invalid_handle_t;
		m_subtextures.clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cspriteatlas::cspriteatlas(texture_t texture) :
		m_texture(texture)
	{
		CORE_ASSERT(algorithm::is_valid_handle(texture), "Invalid operation. Invalid texture handle provided!");
	}

	//------------------------------------------------------------------------------------------------------------------------
	const core::srect& cspriteatlas::get(subtexture_t texture) const
	{
		return m_subtextures[texture];
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cspriteatlas& cspriteatlas::subtexture(const core::srect& rect)
	{
		m_subtextures.push_back(rect);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	texture_t cspriteatlas::atlas() const
	{
		return m_texture;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cspriteatlas& cspriteatlas::end()
	{
		ASSERT(!m_subtextures.empty(), "Empty sprite atlas is not allowed");
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cspriteatlas::subtexture_count() const
	{
		return SCAST(unsigned, m_subtextures.size());
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cspriteatlas& cspriteatlas::begin(texture_t texture)
	{
		m_texture = texture;
		ASSERT(algorithm::is_valid_handle(m_texture), "Creating a sprite atlas with an invalid texture is not allowed");
		return *this;

	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial::cmaterial(ctechnique& shader, technique_t technique, blending_mode mode /*= blending_mode_alpha*/,
		blending_equation equation /*= blending_equation_blend_color*/, blending_factor src /*= blending_factor_src_color*/,
		blending_factor dst /*= blending_factor_one_minus_src_alpha*/) :
		m_shader(shader), m_id(technique), m_blending_mode(mode), m_blending_src(src), m_blending_dst(dst), m_blending_equation(equation)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::bind() const
	{
		shader().bind();

		raylib::rlSetBlendMode(m_blending_mode);
		raylib::rlSetBlendFactors(m_blending_src, m_blending_dst, m_blending_equation);
		raylib::rlSetBlendMode(raylib::BLEND_CUSTOM);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::unbind() const
	{
		raylib::EndBlendMode();

		shader().unbind();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::set_uniform_float(stringview_t name, float value)
	{
		if (shader().ready())
		{
			auto location = shader().uniform_location(name);

			raylib::SetShaderValue(shader().shader(), location, &value, raylib::SHADER_UNIFORM_FLOAT);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::set_uniform_int(stringview_t name, int value)
	{
		if (shader().ready())
		{
			auto location = shader().uniform_location(name);

			raylib::SetShaderValue(shader().shader(), location, &value, raylib::SHADER_UNIFORM_INT);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::set_uniform_vec2(stringview_t name, const vec2_t& value)
	{
		if (shader().ready())
		{
			auto location = shader().uniform_location(name);

			raylib::SetShaderValue(shader().shader(), location, &value, raylib::SHADER_UNIFORM_VEC2);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::set_uniform_vec3(stringview_t name, const vec3_t& value)
	{
		if (shader().ready())
		{
			auto location = shader().uniform_location(name);

			raylib::SetShaderValue(shader().shader(), location, &value, raylib::SHADER_UNIFORM_VEC3);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::set_uniform_vec4(stringview_t name, const vec4_t& value)
	{
		if (shader().ready())
		{
			auto location = shader().uniform_location(name);

			raylib::SetShaderValue(shader().shader(), location, &value, raylib::SHADER_UNIFORM_VEC4);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::set_uniform_matrix(stringview_t name, const mat4_t& value)
	{
		if (shader().ready())
		{
			auto location = shader().uniform_location(name);

			float m[16] = { 0 };

			std::memcpy(&m[0], glm::value_ptr(value), 16 * sizeof(float));

			raylib::SetShaderValueMatrixEx(shader().shader(), location, m);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::set_uniform_texture(stringview_t name, const ctexture& value)
	{
		if (shader().ready())
		{
			auto location = shader().uniform_location(name);

			raylib::SetShaderValueTexture(shader().shader(), location, value.texture());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::set_uniform_texture(stringview_t name, const raylib::Texture& value)
	{
		if (shader().ready())
		{
			auto location = shader().uniform_location(name);

			raylib::SetShaderValueTexture(shader().shader(), location, value);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::remove_uniform(stringview_t name)
	{
		//m_uniforms.erase(name.data());
	}

} //- sm

namespace raylib
{
// 
// 	//------------------------------------------------------------------------------------------------------------------------
// 	Image LoadQOIImage(const core::cpath& path)
// 	{
// 		core::cfile image_file(path);
// 
// 		if (image_file.read_sync() == core::file_io_status_success)
// 		{
// 			const auto [d, s] = image_file.data();
// 
// 			return LoadQOIImageFromMemory(d, s);
// 		}
// 		return { 0 };
// 	}
// 
// 	//------------------------------------------------------------------------------------------------------------------------
// 	Image LoadDDSImage(const core::cpath& path)
// 	{
// 		core::cfile image_file(path);
// 
// 		if (image_file.read_sync() == core::file_io_status_success)
// 		{
// 			const auto [d, s] = image_file.data();
// 
// 			return LoadDDSImageFromMemory(d, s);
// 		}
// 		return { 0 };
// 	}
// 
// 	//------------------------------------------------------------------------------------------------------------------------
// 	Image LoadDDSImageFromMemory(void* image_data, unsigned image_size)
// 	{
// 		auto image = LoadImageFromMemory(".dds", (unsigned char*)image_data, image_size);
// 		if(image.data)
// 		{
// 			return image;
// 		}
// 		return { 0 };
// 	}
// 
// 	//------------------------------------------------------------------------------------------------------------------------
// 	Image LoadQOIImageFromMemory(void* image_data, u32 image_size)
// 	{
// 		Image image;
// 
// 		if (compression::decompress_qoi_to_image(image_data, image_size, image))
// 		{
// 			return image;
// 		}
// 		return { 0 };
// 	}

} //- raylib

namespace compression
{
// 
// 	//------------------------------------------------------------------------------------------------------------------------
// 	sm::texture_format convert_raylib_texture_format(raylib::PixelFormat format)
// 	{
// 		switch(format)
// 		{
// 		case raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8:
// 		{
// 			return sm::texture_format_24bpp;
// 		}
// 		case raylib::PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:
// 		{
// 			return sm::texture_format_8bpp;
// 		}
// 		case raylib::PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:
// 		{
// 			return sm::texture_format_8x2bpp;
// 		}
// 		case raylib::PIXELFORMAT_UNCOMPRESSED_R5G6B5:
// 		{
// 			return sm::texture_format_16bpp;
// 		}
// 		case raylib::PIXELFORMAT_UNCOMPRESSED_R5G5B5A1:
// 		{
// 			return sm::texture_format_16bpp;
// 		}
// 		case raylib::PIXELFORMAT_UNCOMPRESSED_R4G4B4A4:
// 		{
// 			return sm::texture_format_16bpp;
// 		}
// 		case raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
// 		{
// 			return sm::texture_format_32bpp;
// 		}
// 		case raylib::PIXELFORMAT_UNCOMPRESSED_R32G32B32A32:
// 		{
// 			return sm::texture_format_32x4bpp;
// 		}
// 		case raylib::PIXELFORMAT_UNCOMPRESSED_R16G16B16A16:
// 		{
// 			return sm::texture_format_16x4bpp;
// 		}
// 		default:
// 		{
// 			log_warn(fmt::format("Unexpected PixelFormat '{}'", core::algorithm::enum_to_string(format).c_str()));
// 			break;
// 		}
// 		}
// 		return sm::texture_format_none;
// 	}
// 
// 	//------------------------------------------------------------------------------------------------------------------------
// 	u64 estimate_texture_gpu_size(u32 w, u32 h, sm::texture_format format)
// 	{
// 		switch(format)
// 		{
// 		case sm::texture_format_8bpp:
// 		{
// 			//- luminance, i.e. grayscale
// 			return w * h;
// 		}
// 		case sm::texture_format_16bpp:
// 		case sm::texture_format_8x2bpp:
// 		{
// 			//- luminance and alpha, i.e. for fonts
// 			return w * h * 2;
// 		}
// 		case sm::texture_format_24bpp:
// 		{
// 			//- RGB
// 			return w * h * 3;
// 		}
// 		case sm::texture_format_qoi:
// 		case sm::texture_format_32bpp:
// 		{
// 			//- RGBA
// 			return w * h * 4;
// 		}
// 		case sm::texture_format_bc1:
// 		{
// 			//- RGB and binary color, 8:1 compression ratio
// 			return (w * h * 4) / 8;
// 		}
// 		case sm::texture_format_bc3:
// 		case sm::texture_format_bc2:
// 		{
// 			//- RGB and gradient alpha, 4:1 compression ratio
// 			return (w * h * 4) / 4;
// 		}
// 		//- @reference: https://www.cs.cmu.edu/afs/cs/academic/class/15869-f11/www/readings/nystad12_astc.pdf
// 		case sm::texture_format_astc_4x4:
// 		{
// 			//- 8bpp
// 			return w * h;
// 		}
// 		case sm::texture_format_astc_8x8:
// 		{
// 			//- 2bpp
// 			return (w * h) / 4;
// 		}
// 		default:
// 		{
// 			break;
// 		}
// 		}
// 		return 0;
// 	}
// 
// 	//------------------------------------------------------------------------------------------------------------------------
// 	bool compress_image_to_qoi(const core::cpath& path, const raylib::Image& image)
// 	{
// 		qoi_desc desc;
// 		desc.colorspace = QOI_SRGB;
// 		desc.width = image.width;
// 		desc.height = image.height;
// 
// 		switch(image.format)
// 		{
// 		case raylib::PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:
// 		{
// 			desc.channels = 1;
// 			break;
// 		}
// 		case raylib::PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:
// 		{
// 			desc.channels = 2;
// 			break;
// 		}
// 		case raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8:
// 		{
// 			desc.channels = 3;
// 			break;
// 		}
// 		case raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
// 		{
// 			desc.channels = 4;
// 			break;
// 		}
// 		default:
// 		{
// 			//- default RGBA format
// 			desc.channels = 4;
// 			break;
// 		}
// 		}
// 
// 		return qoi_write(path.string().c_str(), image.data, &desc) > 0;
// 	}
// 
// 	//------------------------------------------------------------------------------------------------------------------------
// 	bool compress_image_to_bc(const core::cpath& compressed_image_path, core::cpath& path, stringview_t filename,
// 		nvtt::Format format, nvtt::Quality quality, std::string* error /*= nullptr*/)
// 	{
// 		auto report = [&](const std::string& e)
// 		{
// 			if(error)
// 			{
// 				*error = e;
// 			}
// 		};
// 
// 		if(compressed_image_path.exists())
// 		{
// 			if(path.exists() && path.is_dir())
// 			{
// 				nvtt::Context context(true);
// 				nvtt::OutputOptions output;
// 				nvtt::CompressionOptions options;
// 				nvtt::Surface surface;
// 
// 				if(surface.load(compressed_image_path.string().c_str()))
// 				{
// 					path.append(filename);
// 
// 					output.setFileName(fmt::format("{}.dds", path.string().c_str()).c_str());
// 					options.setFormat(format);
// 					options.setQuality(quality);
// 					context.outputHeader(surface, 1, options, output);
// 					return context.compress(surface, 0, 0, options, output);
// 				}
// 				else
// 				{
// 					report("Source image could not be loaded");
// 				}
// 			}
// 			else
// 			{
// 				report("Invalid destination directory specified");
// 			}
// 		}
// 		else
// 		{
// 			report("Source image file does not exist");
// 		}
// 		return false;
// 	}
// 
// 	//------------------------------------------------------------------------------------------------------------------------
// 	bool decompress_qoi_to_image(const core::cpath& path, raylib::Image& image)
// 	{
// 		qoi_desc desc;
// 
// 		void* data = qoi_read(path.string().c_str(), &desc, 4);
// 		if (data)
// 		{
// 			image.data = data;
// 			image.width = desc.width;
// 			image.height = desc.height;
// 			image.mipmaps = 1;
// 			image.format = raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
// 			return true;
// 		}
// 		return false;
// 	}
// 
// 	//------------------------------------------------------------------------------------------------------------------------
// 	bool decompress_qoi_to_image(void* image_data, u32 image_size, raylib::Image& image)
// 	{
// 		qoi_desc desc;
// 
// 		void* data = qoi_decode(image_data, image_size, &desc, 4);
// 		if (data)
// 		{
// 			image.data = data;
// 			image.width = desc.width;
// 			image.height = desc.height;
// 			image.mipmaps = 1;
// 			image.format = raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
// 			return true;
// 		}
// 		return false;
// 	}

} //- compression