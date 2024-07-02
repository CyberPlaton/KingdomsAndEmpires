#pragma once
#include <core.h>

namespace engine
{
	namespace detail
	{
		constexpr stringview_t C_ASSET_EXTENSION = ".asset";

	} //- detail

	//- Class describing an asset, that is a file accompanying a resource that includes meta information. The asset can
	//- specify any meta data, such as texture compilation options, path to the actual resource, tags given by designers etc.
	//------------------------------------------------------------------------------------------------------------------------
	class casset final
	{
	public:
		using asset_meta_t = umap_t<string_t, rttr::variant>;

		casset();
		casset(stringview_t source_path, rttr::type resource_type);
		casset(stringview_t source_path, rttr::type resource_type, asset_meta_t meta, rttr::variant options);
		~casset() = default;

		stringview_t source_path() const;
		rttr::type resource_type() const;

		template<typename TOptions>
		const TOptions& options() const;

		template<typename TType>
		TType get_meta(stringview_t name, TType default_value);

		template<typename TType>
		void set_meta(stringview_t name, const TType& value);

	private:
		asset_meta_t m_meta;
		string_t m_source_path;
		rttr::type m_resource_type;
		rttr::variant m_options;

		RTTR_REFLECTABLE();
		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TOptions>
	const TOptions& engine::casset::options() const
	{
		return m_options.convert<TOptions>();
	}

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
	REFLECT_INLINE(casset)
	{
		rttr::cregistrator<casset>("casset")
			.prop("m_source_path",		&casset::m_source_path)
			.prop("m_resource_type",	&casset::m_resource_type)
			.prop("m_options",			&casset::m_options)
			.prop("m_meta",				&casset::m_meta)
			;

		rttr::default_constructor<umap_t<string_t, rttr::variant>>();
	};

} //- engine