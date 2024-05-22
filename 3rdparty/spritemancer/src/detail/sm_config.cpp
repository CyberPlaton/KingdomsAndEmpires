#include "sm_config.hpp"

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		bimg::ImageContainer* allocate_image(texture_format_t format, unsigned w, unsigned h, unsigned depth,
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
		static bx::DefaultAllocator S_ALLOCATOR_DEFAULT;
		static cfilereader S_FILEREADER;
		static cfilewriter S_FILEWRITER;
		static cstringwriter S_STRINGWRITER;
		static clogwriter S_LOGWRITER;
		static bx::AllocatorI* S_ALLOCATOR = &S_ALLOCATOR_DEFAULT;
		static ptr_t<iplatform> S_PLATFORM = nullptr;
		static iapp* S_APP = nullptr;
		static ptr_t<irenderer> S_RENDERER = nullptr;

		//------------------------------------------------------------------------------------------------------------------------
		bool cfilereader::open(const bx::FilePath& path, bx::Error* error)
		{
			if (const auto result = bx::FileReader::open(path, error); !result)
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_warn,
						fmt::format("Failed to read from file '{}'", path.getCPtr()));
				}
				return false;
			}
			return true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cfilewriter::open(const bx::FilePath& path, bool append, bx::Error* error)
		{
			if (const auto result = bx::FileWriter::open(path, append, error); !result)
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_warn,
						fmt::format("Failed to write to file '{}'", path.getCPtr()));
				}
				return false;
			}
			return true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::FileReaderI* filereader()
		{
			return &S_FILEREADER;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::FileWriterI* filewriter()
		{
			return &S_FILEWRITER;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::WriterI* stringwriter()
		{
			return &S_STRINGWRITER;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::WriterI* logwriter()
		{
			return &S_LOGWRITER;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::AllocatorI* allocator()
		{
			return S_ALLOCATOR;
		}

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

		//------------------------------------------------------------------------------------------------------------------------
		int32_t cstringwriter::write(const void* data, int32_t size, bx::Error* error)
		{
			if (!error)
			{
				m_string.append((const char*)data, size);
				return -1;
			}
			return size;
		}

		//------------------------------------------------------------------------------------------------------------------------
		int32_t clogwriter::write(const void* data, int32_t size, bx::Error* error)
		{
			if (!error && serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_info,
					(const char*)data);
				return -1;
			}
			return size;
		}

	} //- entry

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::destroy(cshader& shader)
	{
		if (bgfx::isValid(shader.handle()))
		{
			bgfx::destroy(shader.handle());

			shader.m_handle.idx = MAX(uint16_t);
			shader.m_type = shader_type_none;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(shader_type type, stringview_t filepath)
	{
		load_from_file(type, filepath);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(shader_type type, const uint8_t* data, unsigned size)
	{
		load_from_memory(type, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader() :
		m_handle({ MAX(uint16_t) }), m_type(shader_type_none)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::~cshader()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_string(shader_type type, const char* string)
	{
		CORE_ASSERT(false, "Invalid operation. Loading shaders from string is not supported");

		return opresult_fail;
		//return load_from_memory(type, (uint8_t*)string, strlen(string));
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_file(shader_type type, stringview_t filepath)
	{
		CORE_ASSERT(false, "Invalid operation. Loading shaders from file is not supported");

		return opresult_fail;

// 		const auto [data, size] = core::cfile::load_binary(filepath.data());
// 
// 		return load_from_memory(type, data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_memory(shader_type type, const uint8_t* data, unsigned size)
	{
		const bgfx::Memory* mem = bgfx::makeRef(data, size);

		if (m_handle = bgfx::createShader(mem); !bgfx::isValid(m_handle))
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

	//- create a program with a single shader. Can be used for programs without a vertex or fragment shader or a compute shader
	//- program.
	//------------------------------------------------------------------------------------------------------------------------
	bgfx::ProgramHandle cprogram::create(const cshader& shader)
	{
		bgfx::ProgramHandle handle; handle.idx = bgfx::kInvalidHandle;

		if (handle = bgfx::createProgram(shader.handle(), false); !bgfx::isValid(handle))
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
		load_from_handles(m_vertex.handle(), m_fragment.handle());
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
	opresult cprogram::load_from_shaders(const cshader& vertex, const cshader& fragment)
	{
		m_vertex = vertex;
		m_fragment = fragment;
		return load_from_handles(m_vertex.handle(), m_fragment.handle());
	}

	//------------------------------------------------------------------------------------------------------------------------
	opresult cprogram::load_from_handles(bgfx::ShaderHandle vertex, bgfx::ShaderHandle fragment)
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

		m_container = allocate_image(texture_format_t::RGBA8, w, h, 1, 1, false);

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

		m_container = allocate_image(texture_format_t::RGBA8, w, h, 1, 1, false);

		bimg::imageCheckerboard(m_container->m_data, w, h, step, first.rgba(), second.rgba());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctexture::destroy(ctexture& texture)
	{
		if (bgfx::isValid(texture.handle()))
		{
			bgfx::destroy(texture.handle());

			//- reset handle and info
			texture.m_handle.idx = MAX(uint16_t);
			texture.m_info = {};
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture(bgfx::TextureHandle handle, const bgfx::TextureInfo& info) :
		m_handle(handle), m_info(info)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture(const cimage& image)
	{
		load_from_image(image);
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture() :
		m_handle({ MAX(uint16_t)})
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::~ctexture()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult ctexture::load_from_image(const cimage& image)
	{
		const auto& w = image.image()->m_width;
		const auto& h = image.image()->m_height;
		const auto mips = image.image()->m_numMips < 1;
		const auto& layers = image.image()->m_numLayers;
		const auto format = image.image()->m_format;
		const auto& depth = image.image()->m_depth;

		return load_from_memory(image.image()->m_data, image.image()->m_size, w, h, depth, mips, layers,
			texture_format_t(format), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult ctexture::load_from_memory(void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
		bool mips, unsigned layers, texture_format_t format, uint64_t flags)
	{
		const bgfx::Memory* mem = bgfx::makeRef(data, size);

		if (m_handle = bgfx::createTexture2D((uint16_t)w, (uint16_t)h, mips, (uint16_t)layers, format, flags, mem); !bgfx::isValid(m_handle))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading texture");
			}

			return opresult_fail;
		}

		//- calculate amount of memory required for texture
		bgfx::calcTextureSize(m_info, (uint16_t)w, (uint16_t)h, (uint16_t)depth, false, mips, (uint16_t)layers, format);

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void srenderable::destroy(srenderable& renderable)
	{
		ctexture::destroy(renderable.m_texture);
		cimage::destroy(renderable.m_image);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuniform::destroy(cuniform& uniform)
	{
		bgfx::destroy(uniform.handle());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuniform::cuniform(stringview_t name, bgfx::UniformType::Enum type)
	{
		create(name, type);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuniform::cuniform() :
		m_handle({ MAX(uint16_t) })
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuniform::~cuniform()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cuniform::create(stringview_t name, bgfx::UniformType::Enum type)
	{
		m_handle = bgfx::createUniform(name.data(), type, 1);

		return bgfx::isValid(m_handle) ? opresult_ok : opresult_fail;
	}

} //- sm