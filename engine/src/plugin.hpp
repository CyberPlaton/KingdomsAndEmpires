#pragma once
#include <core.h>

namespace engine
{
	//- Class representing a physical plugin located on device, such as a '.dll' or '.dylib'. Responsible for loading and
	//- unloading, as well as containing useful information about the plugin.
	//------------------------------------------------------------------------------------------------------------------------
	class cplugin final
	{
	public:
		using types_t = rttr::array_range<rttr::type>;
		using props_t = rttr::array_range<rttr::property>;
		using methods_t = rttr::array_range<rttr::method>;

		struct sconfig
		{
			string_t m_path;
			string_t m_name;
			string_t m_desc;
			vector_t<string_t> m_dependencies;
			unsigned m_version;

			RTTR_ENABLE();
		};

		cplugin(stringview_t filename);
		cplugin(sconfig&& cfg);
		~cplugin();

		bool load();
		void unload();

		inline const sconfig& config() const { return m_cfg; }
		inline string_t error() const { return m_library.get_error_string().data(); }
		types_t types() const;
		props_t props() const;
		methods_t methods() const;

	private:
		sconfig m_cfg;
		rttr::library m_library;
	};

	using plugin_ref_t = ref_t<cplugin>;

} //- engine