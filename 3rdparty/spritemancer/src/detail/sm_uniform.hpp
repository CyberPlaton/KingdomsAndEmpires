#pragma once
#include "sm_config.hpp"

namespace sm
{
	namespace uniform
	{
		struct suniform_data final
		{
			string_t m_name;
			bgfx::UniformType::Enum m_type;
			unsigned m_data_offset = 0;
		};

	} //- uniform

	//- Lightweight class containing a shader uniform. Can easily be copied around and has to be manually destroyed when
	//- created.
	//------------------------------------------------------------------------------------------------------------------------
	class cuniform final
	{
	public:
		static void destroy(cuniform& uniform);

		explicit cuniform(stringview_t name, bgfx::UniformType::Enum type);
		cuniform();
		~cuniform();

		opresult create(stringview_t name, bgfx::UniformType::Enum type);

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

		material_handle_t load_sync(stringview_t name, stringview_t program, stringview_t texture);
		material_handle_t load_sync(stringview_t name);

		core::cfuture_type<material_handle_t> load_async(stringview_t name, stringview_t program, stringview_t texture);
		core::cfuture_type<material_handle_t> load_async(stringview_t name);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cuniform>);
	};

} //- sm