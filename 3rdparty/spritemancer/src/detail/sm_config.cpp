#include "sm_config.hpp"

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		bimg::ImageContainer* allocate_image(texture_format format, unsigned w, unsigned h, unsigned depth,
			unsigned layers, bool mips, const void* data = nullptr)
		{
			return bimg::imageAlloc(entry::allocator(), bimg::TextureFormat::Enum(format),
				(uint16_t)w, (uint16_t)h, (uint16_t)depth, 1, false, mips, data);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void free_image(bimg::ImageContainer*& container)
		{
			bimg::imageFree(container); container = nullptr;
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
		m_shader({ MAX(uint16_t) }), m_type(shader_type_none)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(shader_type type, stringview_t filepath)
	{
		load_from_file(type, filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(shader_type type, const char* string)
	{
		load_from_string(type, string);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(shader_type type, const uint8_t* data, unsigned size)
	{
		load_from_memory(type, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::~cshader()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_file(shader_type type, stringview_t filepath)
	{
		const auto [data, size] = core::cfile::load_binary(filepath.data());

		return load_from_memory(type, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_string(shader_type type, const char* string)
	{
		return load_from_memory(type, (const uint8_t*)string, strlen(string));
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_memory(shader_type type, const uint8_t* data, unsigned size)
	{
		const bgfx::Memory* mem = bgfx::makeRef(data, size);

		if (m_shader = bgfx::createShader(mem); !bgfx::isValid(m_shader))
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
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bgfx::ProgramHandle cprogram::create(const cshader& shader)
	{
		bgfx::ProgramHandle handle; handle.idx = bgfx::kInvalidHandle;

		if (handle = bgfx::createProgram(shader.shader(), false); !bgfx::isValid(handle))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading program");
			}
		}
		return handle;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprogram::destroy(cprogram& program)
	{
		cshader::destroy(program.m_vertex);
		cshader::destroy(program.m_fragment);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram::cprogram(const cshader& vertex, const cshader& fragment) :
		m_vertex(vertex), m_fragment(fragment)
	{
		load_from_handles(m_vertex.shader(), m_fragment.shader());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram::cprogram() :
		m_handle({ MAX(uint16_t) })
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram::~cprogram()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cprogram::load_from_shaders(const cshader& vertex, const cshader& fragment)
	{
		m_vertex = vertex;
		m_fragment = fragment;
		return load_from_handles(m_vertex.shader(), m_fragment.shader());
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cprogram::load_from_handles(bgfx::ShaderHandle vertex, bgfx::ShaderHandle fragment)
	{
		if (m_handle = bgfx::createProgram(vertex, fragment); !bgfx::isValid(m_handle))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading program");
			}

			return opresult_fail;
		}
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage::destroy(cimage& image)
	{
		free_image(image.m_container);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::cimage(stringview_t filepath)
	{
		load_from_file(filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::cimage(void* data, unsigned size)
	{
		load_from_memory(data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::cimage() :
		m_container(nullptr)
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

		return load_from_memory(data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cimage::load_from_memory(void* data, unsigned size)
	{
		if (m_container = bimg::imageParse(entry::allocator(), data, size); !m_container)
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading image");
			}

			return opresult_fail;
		}
		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage::create_solid(unsigned w, unsigned h, const core::scolor& color)
	{
		if (m_container)
		{
			free_image(m_container);
		}

		m_container = allocate_image(texture_format::RGBA8, w, h, 1, 1, false);

		bimg::imageSolid(m_container->m_data, w, h, color.rgba());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage::create_checkerboard(unsigned w, unsigned h, unsigned step,
		const core::scolor& first, const core::scolor& second)
	{
		if (m_container)
		{
			free_image(m_container);
		}

		m_container = allocate_image(texture_format::RGBA8, w, h, 1, 1, false);

		bimg::imageCheckerboard(m_container->m_data, w, h, step, first.rgba(), second.rgba());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctexture::destroy(ctexture& texture)
	{
		if (bgfx::isValid(texture.texture()))
		{
			bgfx::destroy(texture.texture());

			//- reset handle and info
			texture.m_texture.idx = MAX(uint16_t);
			texture.m_info = {};
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture() :
		m_texture({ MAX(uint16_t) })
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
	ctexture::ctexture(void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
		bool mips, unsigned layers, texture_format format, uint64_t flags)
	{
		load_from_memory(data, size, w, h, depth, mips, layers, format, flags);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture(bgfx::TextureHandle handle, const bgfx::TextureInfo& info) :
		m_texture(handle), m_info(info)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::~ctexture()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult ctexture::load_from_image(const cimage& image)
	{
		const auto w		= image.image()->m_width;
		const auto h		= image.image()->m_height;
		const auto mips		= image.image()->m_numMips < 1;
		const auto& layers	= image.image()->m_numLayers;
		const auto format	= image.image()->m_format;
		const auto& depth	= image.image()->m_depth;

		return load_from_memory(image.image()->m_data, image.image()->m_size, w, h, depth, mips, layers,
			texture_format(format), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult ctexture::load_from_file(stringview_t filepath)
	{
		cimage image(filepath);

		if (is_valid(image) && load_from_image(image))
		{
			return opresult_ok;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult ctexture::load_from_memory(void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
		bool mips, unsigned layers, texture_format format, uint64_t flags)
	{
		cimage image(data, size);

		if (is_valid(image) && load_from_image(image))
		{
			return opresult_ok;
		}

		return opresult_fail;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cshader& shader)
	{
		return bgfx::isValid(shader.shader());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cimage& image)
	{
		return image.image() != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const ctexture& texture)
	{
		return bgfx::isValid(texture.texture());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const crendertarget& target)
	{
		return bgfx::isValid(target.target());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const ccamera& camera)
	{
		return camera.m_ready;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cmaterial& material)
	{
		CORE_ASSERT(false, "Invalid operation. Not supported");

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cspriteatlas& atlas)
	{
		const auto size = atlas.dimension();

		return atlas.subtextures() > 0 && size.x > 0.0f && size.y > 0.0f;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cprogram& program)
	{
		return bgfx::isValid(program.handle());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crendertarget::destroy(crendertarget& target)
	{
		if (is_valid(target))
		{
			bgfx::destroy(target.target());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::crendertarget(unsigned w, unsigned h)
	{
		create(w, h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::crendertarget() :
		m_texture({ MAX(uint16_t) })
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::~crendertarget()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crendertarget::create(unsigned w, unsigned h)
	{
		if (m_framebuffer = bgfx::createFrameBuffer(SCAST(uint16_t, w), SCAST(uint16_t, h),
			texture_format::RGBA8, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP); !bgfx::isValid(m_framebuffer))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading render target");
			}
			return opresult_fail;
		}

		m_texture = bgfx::getTexture(m_framebuffer);
		m_width = SCAST(uint16_t, w);
		m_height = SCAST(uint16_t, h);

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crendertarget::resize(unsigned w, unsigned h)
	{
		crendertarget::destroy(*this);

		return create(w, h);
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
		m_position({ 0.0f, 0.0f }), m_offset({ 0.0f, 0.0f }), m_zoom(0.0f), m_rotation(0.0f), m_ready(false)
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