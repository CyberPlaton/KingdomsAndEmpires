#pragma once
#include <core.h>
#include "../module.hpp"

namespace io
{
	//- Class responsible for loading/unloading and generally managing modules.
	//------------------------------------------------------------------------------------------------------------------------
	class cmodule_service final : public core::cservice
	{
	public:
		cmodule_service() = default;
		~cmodule_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float dt) override final;

		//- Load a module with given type from given filepath. Loading a module involves
		//- creating a new module instance, loading its defined assets and also its dependencies recursively.
		template<typename TModule>
		bool load_module(const core::fs::cfileinfo& filepath);
		bool load_module(stringview_t type_name, const core::fs::cfileinfo& filepath);
		bool load_module(rttr::type type, const core::fs::cfileinfo& filepath);

	private:
		umap_t<string_t, rttr::variant> m_modules;

		core::cmutex m_mutex;

		stack_t<string_t> m_to_load;
		stack_t<string_t> m_to_unload;

		core::ctimer m_timer;

		RTTR_ENABLE(core::cservice);

	private:

	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TModule>
	bool io::cmodule_service::load_module(const core::fs::cfileinfo& filepath)
	{
		return load_module(rttr::type::get<TModule>(), filepath);
	}

} //- io