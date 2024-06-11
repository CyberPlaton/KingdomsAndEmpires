#pragma once
#include <core.h>

namespace engine
{
	//- Class describing an asset, that is a file accompanying a resource that includes meta information. The asset can
	//- specify any meta data, such as texture compilation options, path to the actual resource, tags given by designers etc.
	//------------------------------------------------------------------------------------------------------------------------
	class casset final
	{
	public:
		~casset() = default;

		stringview_t compiler_type() const;

		const rttr::variant& compiling_options() const;

		stringview_t source_path() const;

		stringview_t compiled_path() const;

	private:
		string_t m_compiler_type;
		string_t m_source_path;
		string_t m_compiled_path;
		rttr::variant m_options;

		RTTR_ENABLE();
	};

} //- engine

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(casset)
	{
		rttr::cregistrator<casset>("casset")
			;
	};

} //- engine