#pragma once
#include "sm_config.hpp"
#include "sm_uniform.hpp"

namespace sm
{
	//- TODO: reconsider.
	//------------------------------------------------------------------------------------------------------------------------
	struct ssampler_state final
	{
		cuniform m_sampler;
		unsigned m_sampler_flags = BGFX_SAMPLER_NONE;
	};

	//- A material containing texture data, sampler data and uniforms and a shader program used for drawing geometry.
	//------------------------------------------------------------------------------------------------------------------------
	class cmaterial final : public core::cresource
	{
	public:
		explicit cmaterial(stringview_t program, stringview_t texture);
		explicit cmaterial();
		~cmaterial();

		opresult create(stringview_t program, stringview_t texture);

		//- Note: binding material will submit previously set geometry for rendering with this material.
		//- Any set textures and uniforms will be applied for use with program.
		void bind(uint8_t view = 0, unsigned depth = 0);

		inline texture_handle_t texture() const { return m_texture; }
		inline program_handle_t program() const { return m_program; }

	private:
		vector_t<cuniform> m_uniforms;
		ssampler_state m_sampler_state;
		program_handle_t m_program;
		texture_handle_t m_texture;
		unsigned m_state = BGFX_STATE_DEFAULT;
		bool m_owning_program;
		bool m_owning_texture;

		RTTR_ENABLE(core::cresource);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cmaterial_manager final :
		public core::cservice,
		public core::cresource_manager<cmaterial>
	{
	public:
		cmaterial_manager(unsigned reserve = C_MATERIAL_RESOURCE_MANAGER_RESERVE_COUNT);
		~cmaterial_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		material_handle_t load_sync(stringview_t name, stringview_t program, stringview_t texture);
		material_handle_t load_sync(stringview_t name);

		core::cfuture_type<material_handle_t> load_async(stringview_t name, stringview_t program, stringview_t texture);
		core::cfuture_type<material_handle_t> load_async(stringview_t name);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cmaterial>);
	};

} //- sm
