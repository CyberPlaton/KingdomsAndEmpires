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
		if (m_texture = raylib::LoadTexture(filepath.data()); !raylib::IsTextureReady(m_texture))
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
	void srenderable::destroy(srenderable& renderable)
	{
		ctexture::destroy(renderable.m_texture);
		cimage::destroy(renderable.m_image);
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
	bool is_valid(renderlayer_t layer)
	{
		return layer < MAX(renderlayer_t);
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
	cimage_manager::cimage_manager()
	{
		const auto result = m_images.init(C_IMAGE_RESERVE_COUNT);

		CORE_ASSERT(result, "Invalid operation. Failed initializing image manager!");

		if (!result && serror_reporter::instance().m_callback)
		{
			serror_reporter::instance().m_callback(core::logging_verbosity_critical,
				"Failed initializing image manager!");
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage_manager::~cimage_manager()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	image_t cimage_manager::load(stringview_t name, stringview_t path)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			texture_t handle = invalid_handle_t;

			if (const auto* image = m_images.create(&handle, path); image && is_valid(*image))
			{
				//- image should be loaded at this point, so we can store it
				CORE_ASSERT(image && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading an Image!");

				m_handles[name.data()] = handle;
			}
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	image_t cimage_manager::load(stringview_t name, image_type type, void* data, unsigned size)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			texture_t handle = invalid_handle_t;

			if (const auto* image = m_images.create(&handle, type, data, size); image && is_valid(*image))
			{
				//- image should be loaded at this point, so we can store it
				CORE_ASSERT(image && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading an Image!");

				m_handles[name.data()] = handle;
			}
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	image_t cimage_manager::at(stringview_t name) const
	{
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage_manager::unload(texture_t handle)
	{
		cimage::destroy(get(handle));

		m_images.destroy(handle);

		algorithm::erase_if(m_handles,
			[&](const auto& pair)
			{
				return pair->second == handle;
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	const sm::cimage& cimage_manager::get(image_t handle) const
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_images.find(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cimage& cimage_manager::get(image_t handle)
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_images.modify(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cimage& cimage_manager::get(image_t handle)
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_images.modify(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture_manager::ctexture_manager()
	{
		const auto result = m_textures.init(C_TEXTURE_RESERVE_COUNT);

		CORE_ASSERT(result, "Invalid operation. Failed initializing texture manager!");

		if (!result && serror_reporter::instance().m_callback)
		{
			serror_reporter::instance().m_callback(core::logging_verbosity_critical,
				"Failed initializing texture manager!");
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture_manager::~ctexture_manager()
	{
		m_textures.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	texture_t ctexture_manager::load(stringview_t name, stringview_t path)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			texture_t handle = invalid_handle_t;

			if (const auto* texture = m_textures.create(&handle, path); texture && is_valid(*texture))
			{
				//- texture should be loaded at this point, so we can store it
				CORE_ASSERT(texture && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Texture!");

				m_handles[name.data()] = handle;
			}
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	texture_t ctexture_manager::load(stringview_t name, const cimage& image)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			texture_t handle = invalid_handle_t;

			if (const auto* texture = m_textures.create(&handle, image); texture && is_valid(*texture))
			{
				//- texture should be loaded at this point, so we can store it
				CORE_ASSERT(texture && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Texture!");

				m_handles[name.data()] = handle;
			}
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	texture_t ctexture_manager::load(stringview_t name, image_type type, void* data, unsigned size)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			texture_t handle = invalid_handle_t;

			if (const auto* texture = m_textures.create(&handle, type, data, size); texture && is_valid(*texture))
			{
				//- texture should be loaded at this point, so we can store it
				CORE_ASSERT(texture && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Texture!");

				m_handles[name.data()] = handle;
			}
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	texture_t ctexture_manager::at(stringview_t name) const
	{
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctexture_manager::unload(texture_t handle)
	{
		ctexture::destroy(get(handle));

		m_textures.destroy(handle);

		algorithm::erase_if(m_handles,
			[&](const auto& pair)
			{
				return pair->second == handle;
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	const ctexture& ctexture_manager::get(texture_t handle) const
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_textures.find(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::ctexture& ctexture_manager::get(texture_t handle)
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_textures.modify(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader_manager::cshader_manager()
	{
		const auto result = m_shaders.init(C_TECHNIQUE_RESERVE_COUNT);

		CORE_ASSERT(result, "Invalid operation. Failed initializing shader manager!");

		if (!result && serror_reporter::instance().m_callback)
		{
			serror_reporter::instance().m_callback(core::logging_verbosity_critical,
				"Failed initializing shader manager!");
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader_manager::~cshader_manager()
	{
		m_shaders.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	effect_t cshader_manager::load(stringview_t name, shader_type type, stringview_t vertex_filepath, stringview_t fragment_filepath)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			effect_t handle = invalid_handle_t;

			if (const auto* shader = m_shaders.create(&handle, type, vertex_filepath, fragment_filepath); shader && is_valid(*shader))
			{
				//- shader should be loaded at this point, so we can store it
				CORE_ASSERT(shader && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Shader!");

				m_handles[name.data()] = handle;
			}
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	effect_t cshader_manager::load(stringview_t name, shader_type type, const char* vs, const char* fs)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			effect_t handle = invalid_handle_t;

			if (const auto* shader = m_shaders.create(&handle, type, vs, fs); shader && is_valid(*shader))
			{
				//- shader should be loaded at this point, so we can store it
				CORE_ASSERT(shader && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Shader!");

				m_handles[name.data()] = handle;
			}
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	effect_t cshader_manager::load(stringview_t name, shader_type type, const uint8_t* vs, unsigned vs_size,
		const uint8_t* fs, unsigned fs_size)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			effect_t handle = invalid_handle_t;

			if (const auto* shader = m_shaders.create(&handle, type, vs, vs_size, fs, fs_size); shader && is_valid(*shader))
			{
				//- shader should be loaded at this point, so we can store it
				CORE_ASSERT(shader && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Shader!");

				m_handles[name.data()] = handle;
			}
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cshader_manager::unload(effect_t handle)
	{
		cshader::destroy(get(handle));

		m_shaders.destroy(handle);

		algorithm::erase_if(m_handles,
			[&](const auto& pair)
			{
				return pair->second == handle;
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	effect_t cshader_manager::at(stringview_t name) const
	{
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	const cshader& cshader_manager::get(effect_t handle) const
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_shaders.find(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cshader& cshader_manager::get(effect_t handle)
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_shaders.modify(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_manager::cmaterial_manager()
	{
		const auto result = m_materials.init(C_MATERIAL_RESERVE_COUNT);

		CORE_ASSERT(result, "Invalid operation. Failed initializing material manager!");

		if (!result && serror_reporter::instance().m_callback)
		{
			serror_reporter::instance().m_callback(core::logging_verbosity_critical,
				"Failed initializing material manager!");
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmaterial_manager::~cmaterial_manager()
	{
		m_materials.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	material_t cmaterial_manager::load(stringview_t name, effect_t technique, blending_mode mode /*= blending_mode_alpha*/,
		blending_equation equation /*= blending_equation_blend_color*/, blending_factor src /*= blending_factor_src_color*/,
		blending_factor dst /*= blending_factor_one_minus_src_alpha*/)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			material_t handle = invalid_handle_t;

			//- If shader is not valid we crash here
			auto* sm = core::cservice_manager::find<cshader_manager>();
			auto& shader = sm->get(technique);

			auto* mat = m_materials.create(&handle, shader, technique, mode, equation, src, dst);

			CORE_ASSERT(mat && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Material!");

			m_handles[name.data()] = handle;
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	material_t cmaterial_manager::at(stringview_t name) const
	{
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmaterial_manager::unload(material_t handle)
	{
		cmaterial::destroy(get(handle));

		m_materials.destroy(handle);

		algorithm::erase_if(m_handles,
			[&](const auto& pair)
			{
				return pair->second == handle;
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	const sm::cmaterial& cmaterial_manager::get(material_t handle) const
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_materials.find(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cspriteatlas_manager::cspriteatlas_manager()
	{
		const auto result = m_spritesheets.init(C_SPRITEATLAS_RESERVE_COUNT);

		CORE_ASSERT(result, "Invalid operation. Failed initializing spriteatlas manager!");

		if (!result && serror_reporter::instance().m_callback)
		{
			serror_reporter::instance().m_callback(core::logging_verbosity_critical,
				"Failed initializing spriteatlas manager!");
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cspriteatlas_manager::~cspriteatlas_manager()
	{
		m_spritesheets.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	spriteatlas_t cspriteatlas_manager::load(stringview_t name, texture_t texture, const vec2_t& frames)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			spriteatlas_t handle = invalid_handle_t;

			auto* sheet = m_spritesheets.create(&handle, texture);

			CORE_ASSERT(sheet && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Spriteatlas!");

			m_handles[name.data()] = handle;

			//- initialize sprite atlas. We will crash here if the texture does not exist
			auto* tm = core::cservice_manager::find<ctexture_manager>();
			const auto& tex = tm->get(texture).texture();

			const auto texture_size = vec2_t(tex.width, tex.height);
			const auto frame_size = vec2_t(texture_size.x / frames.x, texture_size.y / frames.y);

			sheet->begin(texture);

			for (auto y = 0u; y < frames.y; ++y)
			{
				for (auto x = 0u; x < frames.x; ++x)
				{
					const auto px = x * frame_size.x / texture_size.x;
					const auto py = y * frame_size.y / texture_size.y;
					const auto w = frame_size.x / texture_size.x;
					const auto h = frame_size.y / texture_size.y;

					sheet->subtexture({ px, py, w, h });
				}
			}

			sheet->end();
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cspriteatlas_manager::unload(material_t handle)
	{
		cspriteatlas::destroy(get(handle));

		m_spritesheets.destroy(handle);

		algorithm::erase_if(m_handles,
			[&](const auto& pair)
			{
				return pair->second == handle;
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	const sm::cspriteatlas& cspriteatlas_manager::get(spriteatlas_t handle) const
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_spritesheets.find(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget_manager::crendertarget_manager()
	{
		const auto result = m_rendertargets.init(C_RENDERTARGET_RESERVE_COUNT);

		CORE_ASSERT(result, "Invalid operation. Failed initializing rendertarget manager!");

		if (!result && serror_reporter::instance().m_callback)
		{
			serror_reporter::instance().m_callback(core::logging_verbosity_critical,
				"Failed initializing rendertarget manager!");
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget_manager::~crendertarget_manager()
	{
		m_rendertargets.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	rendertarget_t crendertarget_manager::load(stringview_t name, unsigned width, unsigned height)
	{
		if (m_handles.find(name.data()) == m_handles.end())
		{
			rendertarget_t handle = invalid_handle_t;

			if (const auto* rt = m_rendertargets.create(&handle, width, height); rt && is_valid(*rt))
			{
				CORE_ASSERT(rt && algorithm::is_valid_handle(handle), "Invalid operation. Failed loading a Rendertarget!");

				m_handles[name.data()] = handle;
			}
		}
		return m_handles.at(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crendertarget_manager::unload(rendertarget_t handle)
	{
		crendertarget::destroy(get(handle));

		m_rendertargets.destroy(handle);

		algorithm::erase_if(m_handles,
			[&](const auto& pair)
			{
				return pair->second == handle;
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	const sm::crendertarget& crendertarget_manager::get(rendertarget_t handle) const
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_rendertargets.find(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::crendertarget& crendertarget_manager::get(rendertarget_t handle)
	{
		CORE_ASSERT(algorithm::is_valid_handle(handle), "Invalid operation. Handle is not valid");

		return *m_rendertargets.modify(handle);
	}

} //- sm