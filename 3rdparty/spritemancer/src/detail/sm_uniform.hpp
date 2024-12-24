#pragma once
#include "sm_config.hpp"

namespace sm
{
    //- Supported uniform types. Note: everything is mapped to a vector4 and a single float uniform will waste 3 floats as unused.
    //- Also there is no direct mapping to bgfx because we extend the functionality.
    //------------------------------------------------------------------------------------------------------------------------
    enum uniform_type : uint8_t
    {
        uniform_type_none = 0,
        uniform_type_float,
        uniform_type_vector2,
        uniform_type_vector3,
        uniform_type_vector4,
        
        uniform_type_mat3x3,
        uniform_type_mat4x4,
        uniform_type_sampler,
    };

	//- Lightweight class containing a shader uniform. Can easily be copied around and has to be manually destroyed when
	//- created. Note: currently uniform packing is not supported, meaning every float will waste 3 floats of data.
	//------------------------------------------------------------------------------------------------------------------------
	class cuniform final
	{
	public:
		explicit cuniform(stringview_t name, uniform_type type);
		cuniform();
		~cuniform();

		opresult create(stringview_t name, uniform_type type);

		uniform_handle_t uniform() const;
		const void* value() const;

		operator bgfx::UniformHandle() const noexcept { return bgfx::UniformHandle{ m_handle }; }

	private:
		core::cany m_value;
		uniform_handle_t m_handle;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cuniform_manager final :
		public core::cservice,
		public core::cresource_manager<cuniform>
	{
	public:
		cuniform_manager(unsigned reserve = C_UNIFORM_RESOURCE_MANAGER_RESERVE_COUNT);
		~cuniform_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

        uniform_handle_t load_sync(stringview_t name, uniform_type type);
        uniform_handle_t load_sync(stringview_t name);

		core::cfuture_type<uniform_handle_t> load_async(stringview_t name, uniform_type type);
		core::cfuture_type<uniform_handle_t> load_async(stringview_t name);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cuniform>);
	};

} //- sm
