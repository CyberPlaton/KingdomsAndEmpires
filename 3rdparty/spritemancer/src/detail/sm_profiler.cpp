#include "sm_profiler.hpp"
#include "sm_resource_manager.hpp"

namespace sm
{
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
				uint64_t shader_gpu_memory_usage(int id);
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

						m_drawcalls.m_textures_bytes += detail::texture_gpu_memory_usage(tex.width, tex.height, tex.mipmaps, (texture_format)tex.format);
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

			//- FIXME: querying shader GPU memory usage directly from OpenGL.
#include <raylib.h>
#if CORE_PLATFORM_DESKTOP
#include <../src/raylib/external/glad.h>
#elif CORE_PLATFORM_MOBILE
#include <../src/raylib/external/glad_gles2.h>
#endif
			//------------------------------------------------------------------------------------------------------------------------
			uint64_t craylib_aggregator::shader_gpu_memory_usage(int id)
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
			m_data({ vertices, 0.0f, name })
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