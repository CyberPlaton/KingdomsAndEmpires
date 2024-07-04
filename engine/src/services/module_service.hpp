#pragma once
#include <core.h>
#include "../module.hpp"

namespace io
{
	//- Class responsible for loading/unloading and generally managing modules.
	//------------------------------------------------------------------------------------------------------------------------
	class cmodule_service final :
		public core::cservice,
		public core::cresource_manager<cmodule>
	{
	public:
		cmodule_service() = default;
		~cmodule_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float dt) override final;

		bool load_sync(const core::fs::cfileinfo& filepath);
		core::cfuture_type<bool> load_async(const core::fs::cfileinfo& filepath);

	private:
		umap_t<string_t, cmodule> m_modules;

		core::cmutex m_mutex;

		stack_t<core::fs::cfileinfo> m_to_load;
		stack_t<core::fs::cfileinfo> m_to_unload;

		core::ctimer m_timer;

		RTTR_ENABLE(core::cservice);

	private:
		bool load_module(const core::fs::cfileinfo& filepath);
	};

} //- io

namespace io
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cmodule_service)
	{
		rttr::cregistrator<cmodule_service, rttr::detail::as_raw_pointer>("cmodule_service")
			;
	};

} //- io