#pragma once
#include "sm_config.hpp"

namespace sm
{
	//- Lightweight class containing a shader uniform. Can easily be copied around and has to be manually destroyed when
	//- created. Note: currently uniform packing is not supported, meaning every float will waste 3 floats of data.
	//------------------------------------------------------------------------------------------------------------------------
	class cuniform final : public core::cresource
	{
	public:
		explicit cuniform(stringview_t name, uniform_type type);
		explicit cuniform(stringview_t name);
		cuniform();
		~cuniform();

		opresult create(stringview_t name, uniform_type type);
		opresult create(stringview_t name);

		uniform_handle_t uniform() const;
		const void* value() const;

		operator bgfx::UniformHandle() const noexcept { return bgfx::UniformHandle{ m_handle }; }

	private:
		string_t m_name;
		core::cany m_value;
		uniform_handle_t m_handle;

		RTTR_ENABLE(core::cresource);
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

        uniform_handle_t load_sync(stringview_t name, stringview_t uniform_name, uniform_type type);
        uniform_handle_t load_sync(stringview_t name, stringview_t uniform_name);

		core::cfuture_type<uniform_handle_t> load_async(stringview_t name, stringview_t uniform_name, uniform_type type);
		core::cfuture_type<uniform_handle_t> load_async(stringview_t name, stringview_t uniform_name);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cuniform>);
	};

} //- sm
