#pragma once
#include <core.h>

namespace engine
{
	namespace detail
	{
		constexpr stringview_t C_ASSET_EXTENSION = ".asset";

	} //- detail

	//- Any meta information for an asset, extendable for any type of asset
	//------------------------------------------------------------------------------------------------------------------------
	struct sasset_meta
	{
		umap_t<string_t, rttr::variant> m_data;

		RTTR_ENABLE();
	};

	//- General asset data, mostly default data, while meta and options depends on the resource type the asset is describing
	//------------------------------------------------------------------------------------------------------------------------
	struct sasset_data
	{
		sasset_meta m_meta;
		string_t m_compiler_type;
		string_t m_source_path;
		string_t m_compiled_path;
		rttr::variant m_options;

		RTTR_ENABLE();
	};

	//- Class describing an asset, that is a file accompanying a resource that includes meta information. The asset can
	//- specify any meta data, such as texture compilation options, path to the actual resource, tags given by designers etc.
	//------------------------------------------------------------------------------------------------------------------------
	class casset final
	{
	public:
		casset(stringview_t compiler_type, stringview_t source_path, stringview_t compiled_path);
		casset(stringview_t compiler_type, stringview_t source_path, stringview_t compiled_path, sasset_meta&& meta, rttr::variant&& options);
		~casset() = default;

		const sasset_data& data() const { return m_data; }

		template<typename TType>
		TType get_meta(stringview_t name, TType default_value);

		template<typename TType>
		void set_meta(stringview_t name, const TType& value);

	private:
		sasset_data m_data;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	void engine::casset::set_meta(stringview_t name, const TType& value)
	{
		m_data.m_meta.m_data[name] = value;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	TType engine::casset::get_meta(stringview_t name, TType default_value)
	{
		if (m_data.m_meta.m_data.find(name) == m_data.m_meta.m_data.end())
		{
			set_meta<TType>(name, default_value);
		}
		return m_data.m_meta.m_data[name].convert<TType>();
	}

} //- engine

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sasset_meta)
	{
		rttr::cregistrator<sasset_meta>("sasset_meta")
			.prop("m_data", &sasset_meta::m_data)
			;
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(sasset_data)
	{
		rttr::cregistrator<sasset_data>("sasset_data")
			.prop("m_meta",			&sasset_data::m_meta)
			.prop("m_compiler_type",&sasset_data::m_compiler_type)
			.prop("m_source_path",	&sasset_data::m_source_path)
			.prop("m_compiled_path",&sasset_data::m_compiled_path)
			.prop("m_options",		&sasset_data::m_options)
			;
	};

} //- engine