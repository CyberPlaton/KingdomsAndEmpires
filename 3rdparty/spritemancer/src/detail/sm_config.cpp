#include "sm_config.hpp"

namespace sm
{
	namespace
	{
		constexpr stringview_t C_PNG = ".png";
		constexpr stringview_t C_BMP = ".bmp";
		constexpr stringview_t C_TGA = ".tga";
		constexpr stringview_t C_JPG = ".jpg";
		constexpr stringview_t C_GIF = ".gif";
		constexpr stringview_t C_PIC = ".pic";
		constexpr stringview_t C_PSD = ".psd";
		constexpr stringview_t C_HDR = ".hdr";
		constexpr stringview_t C_QOI = ".qoi";
		constexpr stringview_t C_SVG = ".svg";
		constexpr stringview_t C_DDS = ".dds";
		constexpr stringview_t C_PKM = ".pkm";
		constexpr stringview_t C_KTX = ".ktx";
		constexpr stringview_t C_PVR = ".pvr";
		constexpr stringview_t C_ASTC = ".astc";

		//------------------------------------------------------------------------------------------------------------------------
		image_type image_type_from_filepath(stringview_t filepath)
		{
			core::cpath path(filepath.data());

			const auto ext = path.extension();

			if (core::string_utils::compare(ext, C_PNG.data()))
			{
				return image_type_png;
			}
			else if (core::string_utils::compare(ext, C_BMP.data()))
			{
				return image_type_bmp;
			}
			else if (core::string_utils::compare(ext, C_TGA.data()))
			{
				return image_type_tga;
			}
			else if (core::string_utils::compare(ext, C_JPG.data()))
			{
				return image_type_jpg;
			}
			else if (core::string_utils::compare(ext, C_GIF.data()))
			{
				return image_type_gif;
			}
			else if (core::string_utils::compare(ext, C_PIC.data()))
			{
				return image_type_pic;
			}
			else if (core::string_utils::compare(ext, C_PSD.data()))
			{
				return image_type_psd;
			}
			else if (core::string_utils::compare(ext, C_HDR.data()))
			{
				return image_type_hdr;
			}
			else if (core::string_utils::compare(ext, C_QOI.data()))
			{
				return image_type_qoi;
			}
			else if (core::string_utils::compare(ext, C_SVG.data()))
			{
				return image_type_svg;
			}
			else if (core::string_utils::compare(ext, C_DDS.data()))
			{
				return image_type_dds;
			}
			else if (core::string_utils::compare(ext, C_PKM.data()))
			{
				return image_type_pkm;
			}
			else if (core::string_utils::compare(ext, C_KTX.data()))
			{
				return image_type_ktx;
			}
			else if (core::string_utils::compare(ext, C_PVR.data()))
			{
				return image_type_pvr;
			}
			else if (core::string_utils::compare(ext, C_ASTC.data()))
			{
				return image_type_astc;
			}

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_warn, fmt::format("Unknown image extension '{}'!", ext));
			}

			return image_type_none;
		}

		//------------------------------------------------------------------------------------------------------------------------
		const char* image_file_type(image_type type)
		{
			switch (type)
			{
			case image_type_png:
			{
#ifdef SUPPORT_FILEFORMAT_PNG
				return C_PNG.data();
#endif
				break;
			}
			case image_type_bmp:
			{
#ifdef SUPPORT_FILEFORMAT_BMP
				return C_BMP.data();
#endif
				break;
			}
			case image_type_tga:
			{
#ifdef SUPPORT_FILEFORMAT_TGA
				return C_TGA.data();
#endif
				break;
			}
			case image_type_jpg:
			{
#ifdef SUPPORT_FILEFORMAT_JPG
				return C_JPG.data();
#endif
				break;
			}
			case image_type_gif:
			{
#ifdef SUPPORT_FILEFORMAT_GIF
				return C_GIF.data();
#endif
				break;
			}
			case image_type_pic:
			{
#ifdef SUPPORT_FILEFORMAT_PIC
				return C_PIC.data();
#endif
				break;
			}
			case image_type_psd:
			{
#ifdef SUPPORT_FILEFORMAT_PSD
				return C_PSD.data();
#endif
				break;
			}
			case image_type_hdr:
			{
#ifdef SUPPORT_FILEFORMAT_HDR
				return C_HDR.data();
#endif
				break;
			}
			case image_type_qoi:
			{
#ifdef SUPPORT_FILEFORMAT_QOI
				return C_QOI.data();
#endif
				break;
			}
			case image_type_svg:
			{
#ifdef SUPPORT_FILEFORMAT_SVG
				return C_SVG.data();
#endif
				break;
			}
			case image_type_dds:
			{
#ifdef SUPPORT_FILEFORMAT_DDS
				return C_DDS.data();
#endif
				break;
			}
			case image_type_pkm:
			{
#ifdef SUPPORT_FILEFORMAT_PKM
				return C_PKM.data();
#endif
				break;
			}
			case image_type_ktx:
			{
#ifdef SUPPORT_FILEFORMAT_KTX
				return C_KTX.data();
#endif
				break;
			}
			case image_type_pvr:
			{
#ifdef SUPPORT_FILEFORMAT_PVR
				return C_PVR.data();
#endif
				break;
			}
			case image_type_astc:
			{
#ifdef SUPPORT_FILEFORMAT_ASTC
				return C_ASTC.data();
#endif
				break;
			}
			default:
				break;
			}

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_warn, fmt::format("Unknown image type '{}'!", algorithm::enum_to_string(type)));
			}

			return nullptr;
		}

	} //- unnamed

	namespace entry
	{
		static ptr_t<iplatform> S_PLATFORM = nullptr;
		static iapp* S_APP = nullptr;
		static ptr_t<irenderer> S_RENDERER = nullptr;

		//------------------------------------------------------------------------------------------------------------------------
		sm::irenderer* renderer()
		{
			return S_RENDERER.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::iplatform* platform()
		{
			return S_PLATFORM.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::iapp* app()
		{
			return S_APP;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_platform(ptr_t<iplatform>&& platform)
		{
			S_PLATFORM = std::move(platform);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_app(iapp* app)
		{
			S_APP = app;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_renderer(ptr_t<irenderer>&& renderer)
		{
			S_RENDERER = std::move(renderer);
		}

	} //- entry

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::destroy(cshader& shader)
	{
		if (is_valid(shader))
		{
			raylib::UnloadShader(shader.shader());
			shader.m_type = shader_type_none;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader() :
		m_shader({ 0 }), m_type(shader_type_none)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(shader_type type, stringview_t vertex_filepath, stringview_t fragment_filepath)
	{
		load_from_file(type, vertex_filepath, fragment_filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(shader_type type, const char* vs, const char* fs)
	{
		load_from_string(type, vs, fs);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(shader_type type, const uint8_t* vs, unsigned vs_size, const uint8_t* fs, unsigned fs_size)
	{
		load_from_memory(type, vs, vs_size, fs, fs_size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::~cshader()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_file(shader_type type, stringview_t vertex_filepath, stringview_t fragment_filepath)
	{
		uint8_t* vs = nullptr; uint8_t* fs = nullptr;
		unsigned vs_size = 0; unsigned fs_size = 0;

		if (!vertex_filepath.empty())
		{
			vs = core::cfile::load_binary(vertex_filepath.data(), vs_size);
		}

		if (!fragment_filepath.empty())
		{
			fs = core::cfile::load_binary(fragment_filepath.data(), fs_size);
		}

		return load_from_memory(type, vs, vs_size, fs, fs_size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_string(shader_type type, const char* vs, const char* fs)
	{
		return load_from_memory(type, (const uint8_t*)vs, strlen(vs), (const uint8_t*)fs, strlen(fs));
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_memory(shader_type type, const uint8_t* vs, unsigned /*vs_size*/, const uint8_t* fs, unsigned /*fs_size*/)
	{
		const char* _vs = nullptr;
		const char* _fs = nullptr;

		switch (type)
		{
		case shader_type_vertex:
		{
			_vs = (const char*)vs;
			break;
		}
		case shader_type_fragment:
		{
			_fs = (const char*)fs;
			break;
		}
		case shader_type_program:
		{
			_vs = (const char*)vs;
			_fs = (const char*)fs;
			break;
		}
		default:
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading shader, unsupported shader type!");
			}
			return opresult_fail;
		}
		}

		if (m_shader = raylib::LoadShaderFromMemory(_vs, _fs); !raylib::IsShaderReady(m_shader))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading shader");
			}

			return opresult_fail;
		}

		m_type = type;

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::set_uniform_float(stringview_t name, float value)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::set_uniform_int(stringview_t name, int value)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::set_uniform_vec2(stringview_t name, const vec2_t& value)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::set_uniform_vec3(stringview_t name, const vec3_t& value)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::set_uniform_vec4(stringview_t name, const vec4_t& value)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::set_uniform_matrix(stringview_t name, const mat4_t& value)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::set_uniform_texture(stringview_t name, const ctexture& value)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::remove_uniform(stringview_t name)
	{
		//- noop
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cshader& cshader::operator=(const cshader& other)
	{
		m_shader = other.m_shader;
		m_type = other.m_type;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage::destroy(cimage& image)
	{
		if (is_valid(image))
		{
			raylib::UnloadImage(image.image());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::cimage(stringview_t filepath)
	{
		load_from_file(filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::cimage(image_type type, void* data, unsigned size)
	{
		load_from_memory(type, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::cimage() :
		m_container({ 0 })
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::~cimage()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cimage::load_from_file(stringview_t filepath)
	{
		const auto [data, size] = core::cfile::load_binary(filepath.data());

		return load_from_memory(image_type_from_filepath(filepath), data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cimage::load_from_memory(image_type type, void* data, unsigned size)
	{
		if (const char* ext = image_file_type(type); ext)
		{
			if (m_container = raylib::LoadImageFromMemory(ext, (const uint8_t*)data, size); raylib::IsImageReady(m_container))
			{
				return opresult_ok;
			}
		}

		if (serror_reporter::instance().m_callback)
		{
			serror_reporter::instance().m_callback(core::logging_verbosity_error, "Failed loading image");
		}

		return opresult_fail;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage::create_solid(unsigned w, unsigned h, const core::scolor& color)
	{
		if (raylib::IsImageReady(m_container))
		{
			raylib::UnloadImage(m_container);
		}

		m_container = raylib::GenImageColor(w, h, to_cliteral(color));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage::create_checkerboard(unsigned w, unsigned h, unsigned step,
		const core::scolor& first, const core::scolor& second)
	{
		if (raylib::IsImageReady(m_container))
		{
			raylib::UnloadImage(m_container);
		}

		m_container = raylib::GenImageChecked(w, h, w / step, h / step, to_cliteral(first), to_cliteral(second));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctexture::destroy(ctexture& texture)
	{
		if (is_valid(texture))
		{
			raylib::UnloadTexture(texture.texture());

			texture.m_texture = { 0 };
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture() :
		m_texture({ 0 })
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture(const cimage& image)
	{
		load_from_image(image);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture(stringview_t filepath)
	{
		load_from_file(filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture(image_type type, void* data, unsigned size)
	{
		load_from_memory(type, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::~ctexture()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult ctexture::load_from_image(const cimage& image)
	{
		if (m_texture = raylib::LoadTextureFromImage(image.image()); !raylib::IsTextureReady(m_texture))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading texture");
			}

			return opresult_fail;
		}

		return opresult_ok;
	}

	sm::opresult ctexture::load_from_file(stringview_t filepath)
	{
		cimage image(filepath);

		if (is_valid(image) && load_from_image(image))
		{
			return opresult_ok;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult ctexture::load_from_memory(image_type type, void* data, unsigned size)
	{
		cimage image(type, data, size);

		if (is_valid(image) && load_from_image(image))
		{
			return opresult_ok;
		}

		return opresult_fail;
	}

	//------------------------------------------------------------------------------------------------------------------------
	raylib::Color to_cliteral(const core::scolor& color)
	{
		return { color.r(), color.g(), color.b(), color.a() };
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cshader& shader)
	{
		return raylib::IsShaderReady(shader.shader());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cimage& image)
	{
		return raylib::IsImageReady(image.image());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const ctexture& texture)
	{
		return raylib::IsTextureReady(texture.texture());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const crendertarget& target)
	{
		return raylib::IsRenderTextureReady(target.target());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const ccamera& camera)
	{
		return camera.ready();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cmaterial& material)
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cspriteatlas& atlas)
	{
		const auto size = atlas.dimension();

		return atlas.subtextures() > 0 && size.x > 0.0f && size.y > 0.0f;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crendertarget::destroy(crendertarget& target)
	{
		if (is_valid(target))
		{
			raylib::UnloadRenderTexture(target.target());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::crendertarget(unsigned w, unsigned h)
	{
		create(w, h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::crendertarget() :
		m_texture({ 0 })
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::~crendertarget()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crendertarget::create(unsigned w, unsigned h)
	{
		if (m_texture = raylib::LoadRenderTexture(w, h); !raylib::IsRenderTextureReady(m_texture))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading render target");
			}

			return opresult_fail;
		}

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccommand::create(render_callback_t&& callback)
	{
		m_callback = std::move(callback);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccommand::execute() const
	{
		m_callback();
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccommand::ccommand(render_callback_t&& callback) :
		m_callback(std::move(callback))
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccamera::ccamera() :
		m_position({ 0 }), m_offset({ 0 }), m_zoom(0.0f), m_rotation(0.0f), m_ready(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	raylib::Camera2D ccamera::camera() const
	{
		return { {m_offset.x, m_offset.y}, {m_position.x, m_position.y}, m_rotation, m_zoom };
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cspriteatlas::destroy(cspriteatlas& /*atlas*/)
	{
		//- noop
	}

	//------------------------------------------------------------------------------------------------------------------------
	cspriteatlas::cspriteatlas(unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames)
	{
		create(w, h, names, frames);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cspriteatlas::cspriteatlas() :
		m_size({0.0f, 0.0f})
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cspriteatlas::create(unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames)
	{
		const auto _w = (float)w;
		const auto _h = (float)h;
		const auto framesize = vec2_t(_w / frames.x, _h / frames.y);

		begin(w, h);

		for (auto y = 0u; y < frames.y; ++y)
		{
			for (auto x = 0u; x < frames.x; ++x)
			{
				const auto px = x * framesize.x / _w;
				const auto py = y * framesize.y / _h;
				const auto pw = framesize.x / _w;
				const auto ph = framesize.y / _h;

				subtexture(names[x + y], { px, py, pw, ph });
			}
		}

		end();

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const core::srect& cspriteatlas::at(stringview_t name) const
	{
		return m_subtextures.at(algorithm::hash(name));
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t cspriteatlas::dimension() const
	{
		return m_size;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cspriteatlas::subtextures() const
	{
		return static_cast<unsigned>(m_subtextures.size());
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cspriteatlas& cspriteatlas::begin(unsigned w, unsigned h)
	{
		m_size = { (float)w, (float)h };
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cspriteatlas& cspriteatlas::subtexture(stringview_t name, const core::srect& rect)
	{
		m_subtextures[algorithm::hash(name)] = rect;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cspriteatlas& cspriteatlas::end()
	{
		CORE_ASSERT(!m_subtextures.empty() && m_size.x > 0.0f && m_size.y > 0.0f, "Invalid operation. An empty spriteatlas is not valid");
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool sblending::operator!=(const sblending& other)
	{
		return !this->operator==(other);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool sblending::operator==(const sblending& other)
	{
		return m_mode == other.m_mode && m_equation == other.m_equation &&
			m_dst_factor == other.m_dst_factor && m_src_factor == other.m_src_factor;
	}

} //- sm