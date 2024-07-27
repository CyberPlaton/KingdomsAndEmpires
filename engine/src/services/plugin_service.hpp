#pragma once
#include <core.h>
#include "../plugin.hpp"

namespace engine
{
	//- Service responsible for loading, unloading and managing active plugins.
	//------------------------------------------------------------------------------------------------------------------------
	class cplugin_service final : public core::cservice
	{
	public:
		cplugin_service() = default;
		~cplugin_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float dt) override final;

		void load_async(stringview_t filename);
		void unload_async(stringview_t filename);

		void load(stringview_t filename);
		void unload(stringview_t filename);

	private:
		stack_t<string_t> m_loading_queue;
		stack_t<string_t> m_unloading_queue;
		umap_t<string_t, plugin_ref_t> m_plugins;
		core::cmutex m_mutex;
		core::ctimer m_timer;

		RTTR_ENABLE(core::cservice);

	private:
		void load_plugin(stringview_t filename);
		void unload_plugin(stringview_t filename);
	};

} //- engine