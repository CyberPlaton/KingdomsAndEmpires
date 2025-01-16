#pragma once
#include "sm_config.hpp"
#include "sm_shader.hpp"

namespace sm
{
	//- Container for vertex and fragment shader. Destroying destroys individual shaders.
	//------------------------------------------------------------------------------------------------------------------------
	class cprogram final : public core::cresource
	{
	public:
		explicit cprogram(shader_handle_t shader);
		explicit cprogram(const cshader& vertex, const cshader& fragment);
		explicit cprogram(shader_handle_t vertex, shader_handle_t fragment);
		cprogram();
		~cprogram();

		opresult load_from_shader(shader_handle_t shader);
		opresult load_from_shaders(const cshader& vertex, const cshader& fragment);
		opresult load_from_handles(shader_handle_t vertex, shader_handle_t fragment);

		inline program_handle_t handle() const { return m_handle; }
		inline const cshader& vertex() const { return m_vertex; }
		inline const cshader& fragment() const { return m_fragment; }

		operator bgfx::ProgramHandle() const noexcept { return bgfx::ProgramHandle{ SCAST(uint16_t, m_handle) }; }

	private:
		cshader m_vertex;
		cshader m_fragment;
		program_handle_t m_handle;

		RTTR_ENABLE(core::cresource);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cprogram_manager final :
		public core::cservice,
		public core::cresource_manager<cprogram>
	{
	public:
		cprogram_manager(unsigned reserve = C_PROGRAM_RESOURCE_MANAGER_RESERVE_COUNT);
		~cprogram_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		program_handle_t load_sync(stringview_t name, shader_handle_t shader);
		program_handle_t load_sync(stringview_t name, shader_handle_t vs, shader_handle_t fs);
		program_handle_t load_sync(stringview_t name, const cshader& vs, const cshader& fs);

		core::cfuture_type<program_handle_t> load_async(stringview_t name, shader_handle_t shader);
		core::cfuture_type<program_handle_t> load_async(stringview_t name, shader_handle_t vs, shader_handle_t fs);
		core::cfuture_type<program_handle_t> load_async(stringview_t name, const cshader& vs, const cshader& fs);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cprogram>);
	};

} //- sm