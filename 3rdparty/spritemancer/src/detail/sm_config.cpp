#include "sm_config.hpp"
#include "sm_resource_manager.hpp"

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

		//------------------------------------------------------------------------------------------------------------------------
		uint64_t texture_gpu_memory_usage(int w, int h, int mips, int format)
		{
			switch(format)
			{
			default:
				break;
			case texture_format_8bpp:
			{
				//- luminance, grayscale, i.e. generally one channel
				return static_cast<uint64_t>(w * h);
			}
			case texture_format_16bpp:
			case texture_format_8x2bpp:
			{
				//- two channels
				return static_cast<uint64_t>(w * h * 2);
			}
			case texture_format_16x4bpp:
			{
				//- RGBA16
				return static_cast<uint64_t>(w * h * 4 * 2);
			}
			case texture_format_24bpp:
			{
				//- RGB
				return static_cast<uint64_t>(w * h * 3);
			}
			case texture_format_qoi:
			case texture_format_32bpp:
			{
				//- RGBA
				return static_cast<uint64_t>(w * h * 4);
			}
			case texture_format_32x4bpp:
			{
				//- RGBA32
				return static_cast<uint64_t>(w * h * 4 * 4);
			}
			case texture_format_bc1:
			{
				//- RGB and binary color, 8:1 compression ratio
				return (w * h * 4) / 8;
			}
			case texture_format_bc3:
			case texture_format_bc2:
			{
				//- RGB and gradient alpha, 4:1 compression ratio
				return (w * h * 4) / 4;
			}
			//- @reference: https://www.cs.cmu.edu/afs/cs/academic/class/15869-f11/www/readings/nystad12_astc.pdf
			case texture_format_astc_4x4:
			{
				//- 8bpp
				return w * h;
			}
			case texture_format_astc_8x8:
			{
				//- 2bpp
				return (w * h) / 4;
			}
			}
			return 0;
		}

		//- FIXME: only while we are using raylib
	#include <raylib.h>
#if CORE_PLATFORM_DESKTOP
	#include <../src/raylib/external/glad.h>
#elif CORE_PLATFORM_MOBILE
	#include <../src/raylib/external/glad_gles2.h>
#endif
		//------------------------------------------------------------------------------------------------------------------------
		uint64_t shader_gpu_memory_usage(int id)
		{
			int binary_size = 0;
			int shader_buffer_size = 0;

			//- Get shader binary size
			glGetProgramiv(id, GL_PROGRAM_BINARY_LENGTH, &binary_size);

			//- Get uniforms and their storage size
			shader_buffer_size = raylib::rlGetShaderBufferSize(id);

			return static_cast<uint64_t>(shader_buffer_size + binary_size);
		}

	} //- unnamed

	namespace entry
	{
		static ptr_t<ios> S_OS = nullptr;
		static iapp* S_APP = nullptr;
		static ptr_t<irenderer> S_RENDERER = nullptr;
		static ptr_t<iplatform> S_PLATFORM = nullptr;

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
		memory_ref_t vs = nullptr, fs = nullptr;

		if (!vertex_filepath.empty())
		{
			vs = core::fs::load_text_from_file(vertex_filepath);
		}

		if (!fragment_filepath.empty())
		{
			fs = core::fs::load_text_from_file(fragment_filepath);
		}

		return load_from_memory(type, (uint8_t*)vs->data(), (unsigned)vs->size(), (uint8_t*)fs->data(), (unsigned)fs->size());
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
		return *this;
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
	cimage::cimage(image_generate_function_t&& callback) :
		m_container(callback())
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cimage::~cimage()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult cimage::load_from_file(stringview_t filepath)
	{
		const auto memory = core::fs::load_binary_from_file(filepath.data());

		return load_from_memory(image_type_from_filepath(filepath), (void*)memory->data(), (unsigned)memory->size());
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
	bool srenderstate::operator==(const srenderstate& other) const
	{
		return m_blending == other.m_blending && m_flags == other.m_flags;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool srenderstate::operator!=(const srenderstate& other) const
	{
		return !this->operator==(other);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool sblending::operator!=(const sblending& other) const
	{
		return !this->operator==(other);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool sblending::operator==(const sblending& other) const
	{
		return m_mode == other.m_mode && m_equation == other.m_equation &&
			m_dst_factor == other.m_dst_factor && m_src_factor == other.m_src_factor;
	}

	namespace profile::gpu
	{
		namespace
		{
			static core::cmutex S_MUTEX;
			static aggregator_ref_t S_AGGREGATOR = nullptr;

			//- GPU aggregator for raylib
			//------------------------------------------------------------------------------------------------------------------------
			class craylib_aggregator final : public iaggregator
			{
			public:
				craylib_aggregator() = default;
				~craylib_aggregator() = default;

				vector_t<sgpu_stats>		stats() override final;
				saggregated_drawcall_data	drawcall_data() override final;
				void						update() override final;
				void						push(gpu::sdrawcall_data&& data) override final;

			private:
				saggregated_drawcall_data m_drawcalls;
				core::stack_t<sdrawcall_data> m_stack;
				vector_t<sgpu_stats> m_current;

			private:
				void gather_gpu_information();
				void gather_drawcall_information();
			};

			//------------------------------------------------------------------------------------------------------------------------
			vector_t<sgpu_stats> craylib_aggregator::stats()
			{
				core::cscope_mutex m(S_MUTEX);

				return m_current;
			}

			//------------------------------------------------------------------------------------------------------------------------
			void craylib_aggregator::update()
			{
				core::cscope_mutex m(S_MUTEX);

				gather_gpu_information();
				gather_drawcall_information();
			}

			//------------------------------------------------------------------------------------------------------------------------
			void craylib_aggregator::gather_gpu_information()
			{
				m_current.clear();

				for (const auto& info : hwinfo::getAllGPUs())
				{
					sgpu_stats stat;

					stat.m_model_vendor_driver = fmt::format("{}-{}-{}", info.vendor(), info.name(), info.driverVersion());
					stat.m_cores = static_cast<int64_t>(info.num_cores());
					stat.m_clock_speed = info.frequency_MHz();
					stat.m_memory = info.memory_Bytes();

					m_current.push_back(stat);
				}
			}

			//------------------------------------------------------------------------------------------------------------------------
			void craylib_aggregator::push(gpu::sdrawcall_data&& data)
			{
				core::cscope_mutex m(S_MUTEX);

				m_stack.push(std::move(data));
			}

			//------------------------------------------------------------------------------------------------------------------------
			void craylib_aggregator::gather_drawcall_information()
			{
				//- First gather information about draw calls and their time
				while (!m_stack.empty())
				{
					const auto& drawcall = m_stack.top(); m_stack.pop();

					++m_drawcalls.m_drawcalls;
					m_drawcalls.m_vertices += drawcall.m_vertices;
					m_drawcalls.m_drawing_time_total += drawcall.m_time;
					m_drawcalls.m_drawing_time_mean = (m_drawcalls.m_drawing_time_total / static_cast<float>(m_drawcalls.m_drawcalls));
				}

				//- Second gather information about textures and shaders and their used GPU memory
				auto& tm = core::cservice_manager::get<ctexture_manager>();
				auto& sm = core::cservice_manager::get<cshader_manager>();

				m_drawcalls.m_textures_count = tm.count();
				m_drawcalls.m_shaders_count = sm.count();

				tm.each([&](const ctexture& texture)
					{
						auto tex = texture.texture();

						m_drawcalls.m_textures_bytes += texture_gpu_memory_usage(tex.width, tex.height, tex.mipmaps, tex.format);
					});

				sm.each([&](const cshader& shader)
					{
						m_drawcalls.m_shaders_bytes += shader_gpu_memory_usage(shader.shader().id);
					});

				//- TODO: as of now we ignore the GPU memory used by rendertarget textures
			}

			//------------------------------------------------------------------------------------------------------------------------
			saggregated_drawcall_data craylib_aggregator::drawcall_data()
			{
				core::cscope_mutex m(S_MUTEX);

				return m_drawcalls;
			}

		} //- unnamed

		//------------------------------------------------------------------------------------------------------------------------
		void set_aggregator(aggregator_ref_t object)
		{
			S_AGGREGATOR = object;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_default_aggregator()
		{
			S_AGGREGATOR = std::make_shared<craylib_aggregator>();
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
