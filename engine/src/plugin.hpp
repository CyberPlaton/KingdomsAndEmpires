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
		struct sinfo
		{
			string_t m_path;
			string_t m_name;
			string_t m_desc;
			vector_t<string_t> m_dependencies;
			unsigned m_version;

			RTTR_ENABLE();
		};

		cplugin(stringview_t filename);
		cplugin(sinfo&& info);
		~cplugin();

		bool load();
		void unload();

		inline const sinfo&					info() const { return m_info; }
		inline string_t						error() const { return m_library.get_error_string().data(); }
		rttr::array_range<rttr::type>		types() const;
		rttr::array_range<rttr::property>	props() const;
		rttr::array_range<rttr::method>		methods() const;

	private:
		sinfo m_info;
		rttr::library m_library;
	};

	using plugin_ref_t = ref_t<cplugin>;

} //- engine