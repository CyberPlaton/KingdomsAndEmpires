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
	cwindow::~cwindow()
	{
		raylib::CloseWindow();
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
	cwindow::cwindow(const sconfig& cfg)
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
	csprite_atlas::csprite_atlas(const csprite_atlas& other) :
		m_texture(other.m_texture), m_subtextures(other.m_subtextures)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::csprite_atlas& csprite_atlas::operator=(const csprite_atlas& other)
	{
		m_texture = other.m_texture;
		m_subtextures = other.m_subtextures;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void csprite_atlas::reset()
	{
		m_texture = 0;
		m_subtextures.clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::srect csprite_atlas::subtexture(subtexture_t texture) const
	{
		return m_subtextures[texture];
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::csprite_atlas& csprite_atlas::subtexture(const core::srect& rect)
	{
		m_subtextures.push_back(rect);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	texture_t csprite_atlas::atlas() const
	{
		return m_texture;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::csprite_atlas& csprite_atlas::end()
	{
		ASSERT(!m_subtextures.empty(), "Empty sprite atlas is not allowed");
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned csprite_atlas::subtexture_count() const
	{
		return SCAST(unsigned, m_subtextures.size());
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::csprite_atlas& csprite_atlas::begin(texture_t texture)
	{
		m_texture = texture;
		ASSERT(algorithm::is_valid_handle(m_texture), "Creating a sprite atlas with an invalid texture is not allowed");
		return *this;

	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader_uniform::cshader_uniform(const std::string& name, void* value, raylib::ShaderUniformDataType type) :
		m_name(name), m_data(value), m_type(type)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void* cshader_uniform::get() const
	{
		return m_data;
	}

	//------------------------------------------------------------------------------------------------------------------------
	raylib::ShaderUniformDataType cshader_uniform::get_type() const
	{
		return m_type;
	}

	//------------------------------------------------------------------------------------------------------------------------
	std::string cshader_uniform::get_name() const
	{
		return m_name;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const char* cshader_uniform::c_str() const
	{
		return m_name.c_str();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial::cmaterial(technique_t technique, blending_mode mode, blending_equation equation, blending_factor src, blending_factor dst) :
		m_program(technique), m_blending_mode(mode), m_blending_equation(equation), m_blending_src(src), m_blending_dst(dst)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial::cmaterial(technique_t technique, blending_mode mode, blending_equation equation, blending_factor src, blending_factor dst, const vector_t< cshader_uniform >& static_uniforms, const vector_t< cshader_uniform >& dynamic_uniforms) :
		m_program(technique), m_blending_mode(mode), m_blending_equation(equation), m_blending_src(src), m_blending_dst(dst), m_static_uniforms(static_uniforms), m_dynamic_uniforms(dynamic_uniforms)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial::~cmaterial()
	{
		//- cmaterial is not responsible for unloading the shader and texture from memory
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::bind_shader(const raylib::Shader& technique) const
	{
		if (IsShaderReady(technique))
		{
			raylib::BeginShaderMode(technique);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::bind_blend_mode() const
	{
		raylib::rlSetBlendMode(m_blending_mode);
		raylib::rlSetBlendFactors(m_blending_src, m_blending_dst, m_blending_equation);
		raylib::rlSetBlendMode(raylib::BLEND_CUSTOM);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::reset()
	{
		m_static_uniforms.clear();
		m_dynamic_uniforms.clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::bind_static_uniforms(const raylib::Shader& technique) const
	{
		bind_uniforms(technique, m_static_uniforms);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::bind_dynamic_uniforms(const raylib::Shader& technique) const
	{
		bind_uniforms(technique, m_dynamic_uniforms);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::end_shader() const
	{
		raylib::EndShaderMode();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::end_blend_mode() const
	{
		raylib::EndBlendMode();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::set_blend_mode(blending_mode mode)
	{
		m_blending_mode = mode;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::set_blend_mode_src_factor(blending_factor factor)
	{
		m_blending_src = factor;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::set_blend_mode_dst_factor(blending_factor factor)
	{
		m_blending_dst = factor;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::set_blend_mode_equation(blending_equation equation)
	{
		m_blending_equation = equation;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::restore_default_blend_mode()
	{
		m_blending_mode = blending_mode_alpha;
		m_blending_src = blending_factor_src_alpha;
		m_blending_dst = blending_factor_one_minus_src_alpha;
		m_blending_equation = blending_equation_blend_color;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::set_shader(technique_t technique)
	{
		m_program = technique;
	}

	//------------------------------------------------------------------------------------------------------------------------
	technique_t cmaterial::get_shader() const
	{
		return m_program;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial::bind_uniforms(const raylib::Shader& technique, const vector_t< cshader_uniform >& vector) const
	{
		if (raylib::IsShaderReady(technique))
		{
			uint32_t uniform_type = 0;
			uint32_t uniform_loc = 0;

			for (const auto& uniform : vector)
			{
				uniform_loc = raylib::GetShaderLocation(technique, uniform.c_str());

				switch (uniform.get_type())
				{
				case raylib::ShaderUniformDataType::SHADER_UNIFORM_FLOAT:
				{
					uniform_type = raylib::SHADER_UNIFORM_FLOAT;
					break;
				}
				case raylib::ShaderUniformDataType::SHADER_UNIFORM_INT:
				{
					uniform_type = raylib::SHADER_UNIFORM_INT;
					break;
				}
				case raylib::ShaderUniformDataType::SHADER_UNIFORM_IVEC2:
				case raylib::ShaderUniformDataType::SHADER_UNIFORM_VEC2:
				{
					uniform_type = raylib::SHADER_UNIFORM_VEC2;
					break;
				}
				case raylib::ShaderUniformDataType::SHADER_UNIFORM_IVEC3:
				case raylib::ShaderUniformDataType::SHADER_UNIFORM_VEC3:
				{
					uniform_type = raylib::SHADER_UNIFORM_VEC3;
					break;
				}
				case raylib::ShaderUniformDataType::SHADER_UNIFORM_IVEC4:
				case raylib::ShaderUniformDataType::SHADER_UNIFORM_VEC4:
				{
					uniform_type = raylib::SHADER_UNIFORM_VEC4;
					break;
				}
				case raylib::ShaderUniformDataType::SHADER_UNIFORM_SAMPLER2D:
				{
					uniform_type = raylib::SHADER_UNIFORM_SAMPLER2D;
					break;
				}
				default:
				{
					continue;
				}
				}
				raylib::SetShaderValue(technique, uniform_loc, uniform.get(), uniform_type);
			}
		}
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