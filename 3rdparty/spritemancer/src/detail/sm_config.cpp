#include "sm_config.hpp"
#include "sm_resource_manager.hpp"

namespace sm
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		class cfilereader final : public bx::FileReader
		{
		public:
			bool open(const bx::FilePath& path, bx::Error* error) override final
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
		};

		//- Note: can be use anywhere, where bx::WriterI* is required, i.e. when you want to compile shaders as binary files
		//------------------------------------------------------------------------------------------------------------------------
		class cfilewriter final : public bx::FileWriter
		{
		public:
			bool open(const bx::FilePath& path, bool append, bx::Error* error) override final
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
		};

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
		static ptr_t<ios> S_OS = nullptr;
		static iapp* S_APP = nullptr;
		static ptr_t<irenderer> S_RENDERER = nullptr;
		static ptr_t<iplatform> S_PLATFORM = nullptr;
		static bx::DefaultAllocator S_ALLOCATOR;
		static cfilereader S_FILEREADER;
		static cfilewriter S_FILEWRITER;

		//------------------------------------------------------------------------------------------------------------------------
		bx::FileReaderI* filreader()
		{
			return &S_FILEREADER;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::FileWriterI* filewriter()
		{
			return &S_FILEWRITER;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bx::AllocatorI* allocator()
		{
			return &S_ALLOCATOR;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool has_platform()
		{
			return S_PLATFORM.get() != nullptr;
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::iplatform* get_platform()
		{
			return S_PLATFORM.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::irenderer* get_renderer()
		{
			return S_RENDERER.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::ios* get_os()
		{
			return S_OS.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::iapp* get_app()
		{
			return S_APP;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_os(ptr_t<ios>&& os)
		{
			S_OS = std::move(os);
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

	namespace vertex_layouts
	{
		//------------------------------------------------------------------------------------------------------------------------
		bool spostexcoord::init()
		{
			S_LAYOUT.begin()
				.add(bgfx::Attrib::Position,	3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0,	2, bgfx::AttribType::Float)
				.end();

			S_HANDLE = bgfx::createVertexLayout(S_LAYOUT);

			return bgfx::isValid(S_HANDLE);
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool spostexcoordcolor::init()
		{
			S_LAYOUT.begin()
				.add(bgfx::Attrib::Position,	3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0,	2, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Color0,		4, bgfx::AttribType::Uint8, true)
				.end();

			S_HANDLE = bgfx::createVertexLayout(S_LAYOUT);

			return bgfx::isValid(S_HANDLE);
		}

	} //- vertex_layouts

	//------------------------------------------------------------------------------------------------------------------------
	cvertices::cvertices(const bgfx::VertexLayout& layout, uint64_t count) :
		m_vertex_layout(layout)
	{
		m_data.resize(m_vertex_layout.getSize(SCAST(unsigned, count)));
	}

	//------------------------------------------------------------------------------------------------------------------------
	cvertices::cvertices() = default;

	//------------------------------------------------------------------------------------------------------------------------
	void cvertices::init(const bgfx::VertexLayout& layout, uint64_t count)
	{
		m_vertex_layout = layout;
		m_data.resize(m_vertex_layout.getSize(SCAST(unsigned, count)));
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cvertices& cvertices::begin(uint64_t idx)
	{
		m_idx = SCAST(unsigned, idx);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cvertices& cvertices::position(const vec3_t& v)
	{
		CORE_ASSERT(m_idx != -1, "Invalid operation");

		bgfx::vertexPack(&v.x, false, bgfx::Attrib::Position, m_vertex_layout, m_data.data(), m_idx);
		m_vertex_attribute_flags |= BIT(bgfx::Attrib::Position);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cvertices& cvertices::tex_coord(const vec2_t& v)
	{
		CORE_ASSERT(m_idx != -1, "Invalid operation");

		bgfx::vertexPack(&v.x, true, bgfx::Attrib::TexCoord0, m_vertex_layout, m_data.data(), m_idx);
		m_vertex_attribute_flags |= BIT(bgfx::Attrib::TexCoord0);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cvertices& cvertices::color(const core::scolor& v)
	{
		CORE_ASSERT(m_idx != -1, "Invalid operation");

		auto c = v.normalize();

		bgfx::vertexPack(&c.x, true, bgfx::Attrib::Color0, m_vertex_layout, m_data.data(), m_idx);
		m_vertex_attribute_flags |= BIT(bgfx::Attrib::Color0);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cvertices& cvertices::end()
	{
		CORE_ASSERT(m_idx != -1, "Invalid operation");

		static constexpr const char* C_ATTRIBUTE_NAMES[bgfx::Attrib::Count] =
		{
			"Position", "Normal", "Tangent", "Bitangent", "Color0", "Color1", "Color2", "Color3", "Indices",
			"Weight", "TexCoord0", "TexCoord1", "TexCoord2", "TexCoord3", "TexCoord4", "TexCoord5", "TexCoord6", "TexCoord7"
		};

		//- Validate correct declaration and usage
		for (auto i = 0; i < (int)bgfx::Attrib::Count; ++i)
		{
			if (m_vertex_layout.has((bgfx::Attrib::Enum)i) &&
				!algorithm::bit_check(m_vertex_attribute_flags, BIT((bgfx::Attrib::Enum)i)))
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(core::logging_verbosity_warn,
						fmt::format("Incomplete vertices declaration! Missing attribute '{}' at index '{}'!",
							C_ATTRIBUTE_NAMES[i], m_idx));
				}
			}
		}

		m_idx = -1;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuniform::destroy(cuniform& uniform)
	{
		if (is_valid(uniform))
		{
			bgfx::destroy(uniform.uniform());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuniform::cuniform(stringview_t name, bgfx::UniformType::Enum type)
	{
		create(name, type);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuniform::cuniform() : m_handle({ bgfx::kInvalidHandle })
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

	//------------------------------------------------------------------------------------------------------------------------
	void cshader::destroy(cshader& shader)
	{
		if (is_valid(shader))
		{
			bgfx::destroy(shader.shader());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader() :
		m_handle({ bgfx::kInvalidHandle })
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(stringview_t path)
	{
		load_from_file(path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(const char* text)
	{
		load_from_string(text);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::cshader(const uint8_t* data, unsigned size)
	{
		load_from_memory(data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cshader::~cshader()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_file(stringview_t path)
	{
		memory_ref_t data = nullptr;

		if (!path.empty())
		{
			data = core::fs::load_text_from_file(path);
		}

		return load_from_memory((uint8_t*)data->data(), (unsigned)data->size());
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_string(const char* text)
	{
		return load_from_memory((const uint8_t*)text, strlen(text));
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cshader::load_from_memory(const uint8_t* data, unsigned size)
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

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::cshader& cshader::operator=(const cshader& other)
	{
		m_handle = other.m_handle;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cprogram::destroy(cprogram& program)
	{
		if (is_valid(program))
		{
			bgfx::destroy(program.program());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram::cprogram(const bgfx::ShaderHandle vs, const bgfx::ShaderHandle fs)
	{
		load_from_handles(vs, fs);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram::cprogram(const cshader& vs, const cshader& fs)
	{
		load_from_shaders(vs, fs);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram::cprogram() : m_program({ bgfx::kInvalidHandle })
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cprogram::~cprogram()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	opresult cprogram::load_from_shaders(const cshader& vs, const cshader& fs)
	{
		return load_from_handles(vs.shader(), fs.shader());
	}

	//------------------------------------------------------------------------------------------------------------------------
	opresult cprogram::load_from_handles(const bgfx::ShaderHandle vs, const bgfx::ShaderHandle fs)
	{
		if (m_program = bgfx::createProgram(vs, fs, false); !bgfx::isValid(m_program))
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
	cprogram& cprogram::operator=(const cprogram& other)
	{
		m_program = other.m_program;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cimage::destroy(cimage& image)
	{
		if (is_valid(image))
		{
			free_image(image.m_container);
		}
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
		auto mem = core::fs::load_binary_from_file(filepath.data());

		return load_from_memory(mem->data(), mem->size());
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
		if (is_valid(texture))
		{
			bgfx::destroy(texture.texture());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	ctexture::ctexture() :
		m_handle({ bgfx::kInvalidHandle })
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
			texture_format(format), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult ctexture::load_from_file(stringview_t filepath)
	{
		cimage image(filepath);

		return load_from_image(image);
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult ctexture::load_from_memory(void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
		bool mips, unsigned layers, texture_format format, uint64_t flags)
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

		bgfx::calcTextureSize(m_info, (uint16_t)w, (uint16_t)h, (uint16_t)depth, false, mips, (uint16_t)layers, format);

		m_uniform.create(C_UNIFORM_TEXTURE0, bgfx::UniformType::Sampler);

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cshader& shader)
	{
		return bgfx::isValid(shader.shader());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cuniform& uniform)
	{
		return bgfx::isValid(uniform.uniform());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cprogram& program)
	{
		return bgfx::isValid(program.program());
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
		m_target({ bgfx::kInvalidHandle })
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::~crendertarget()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crendertarget::create(unsigned w, unsigned h)
	{
		if (m_target = bgfx::createFrameBuffer(w, h, bgfx::TextureFormat::RGBA8,
			BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP); !bgfx::isValid(m_target))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading render target");
			}

			return opresult_fail;
		}

		bgfx::calcTextureSize(m_info, w, h, 0, false, false, 0, bgfx::TextureFormat::RGBA8);

		m_uniform.create(C_UNIFORM_TEXTURE0, bgfx::UniformType::Sampler);

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
		m_translation({0}), m_rotation({0}), m_scale({1}),
		m_viewport(0.0f, 0.0f, 1.0f, 1.0f), m_near(0.0001f), m_far(MAX(float)), m_zoom(1.0f),
		m_clear_color(core::common_color_neutral1000), m_state(BGFX_STATE_BLEND_ALPHA | BGFX_STATE_DEFAULT),
		m_clear_state(BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH), m_window_handle(nullptr)
	{
		matrix_view_update();
		matrix_projection_update();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccamera::matrix_view_update()
	{
		m_view =  glm::translate(mat4_t(1.0f), -vec3_t(m_translation))
			* glm::rotate(mat4_t(1.0f), glm::radians(m_rotation.z), vec3_t(0.0f, 0.0f, 1.0f))
			* glm::scale(mat4_t(1.0f), vec3_t(vec2_t(m_scale), 1.0f));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccamera::matrix_projection_update()
	{
		m_projection = glm::ortho(-m_zoom, m_zoom, -m_zoom, m_zoom);
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
	bool srenderstate::operator==(const srenderstate& other) const
	{
		return m_blending == other.m_blending;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool srenderstate::operator!=(const srenderstate& other) const
	{
		return !this->operator==(other);
	}

	namespace profile::gpu
	{
		namespace
		{
			static core::cmutex S_MUTEX;
			static aggregator_ref_t S_AGGREGATOR = nullptr;

		} //- unnamed

		//------------------------------------------------------------------------------------------------------------------------
		void set_aggregator(aggregator_ref_t object)
		{
			S_AGGREGATOR = object;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_default_aggregator()
		{
			/*S_AGGREGATOR = std::make_shared<craylib_aggregator>();*/
		}

		//------------------------------------------------------------------------------------------------------------------------
		aggregator_ref_t get_aggregator()
		{
			return S_AGGREGATOR;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cscoped_drawcall::cscoped_drawcall(uint64_t vertices, const char* name) :
			m_data({vertices, 0.0f, name})
		{
			m_timer.start();
		}

		//------------------------------------------------------------------------------------------------------------------------
		cscoped_drawcall::~cscoped_drawcall()
		{
			m_data.m_time = m_timer.microsecs();

			cprofiler::push(std::move(m_data));
		}

	} //- profile::gpu

	namespace profile
	{
		//------------------------------------------------------------------------------------------------------------------------
		void cprofiler::push(gpu::sdrawcall_data&& data)
		{
			gpu::get_aggregator()->push(std::move(data));
		}

		//------------------------------------------------------------------------------------------------------------------------
		gpu::saggregated_drawcall_data cprofiler::drawcall_data()
		{
			return gpu::get_aggregator()->drawcall_data();
		}

	} //- profile

} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<cspriteatlas>("cspriteatlas")
		.meth(core::cresource::C_DESTROY_FUNCTION_NAME.data(), &cspriteatlas::destroy)
		.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS,
			vector_t<string_t>{".png", ".bmp", ".tga", ".jpg", ".gif", ".pic",
			".psd", ".hdr", ".qoi", ".svg", ".dds", ".pkm", ".ktx", ".pvr", ".astc"})
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<crendertarget>("crendertarget")
		.meth(core::cresource::C_DESTROY_FUNCTION_NAME.data(), &crendertarget::destroy)
		.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS, vector_t<string_t>{})
	;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<ctexture>("ctexture")
		.meth(core::cresource::C_DESTROY_FUNCTION_NAME.data(), &ctexture::destroy)
		.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS,
			vector_t<string_t>{".png", ".bmp", ".tga", ".jpg", ".gif", ".pic",
			".psd", ".hdr", ".qoi", ".svg", ".dds", ".pkm", ".ktx", ".pvr", ".astc"})
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<cimage>("cimage")
		.meth(core::cresource::C_DESTROY_FUNCTION_NAME.data(), &cimage::destroy)
		.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS,
			vector_t<string_t>{".png", ".bmp", ".tga", ".jpg", ".gif", ".pic",
			".psd", ".hdr", ".qoi", ".svg", ".dds", ".pkm", ".ktx", ".pvr", ".astc"})
		;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<cshader>("cshader")
		.meth(core::cresource::C_DESTROY_FUNCTION_NAME.data(), &cshader::destroy)
		.meta(core::cresource::C_META_SUPPORTED_EXTENSIONS, vector_t<string_t>{".vs", ".fs"})
	;
}
