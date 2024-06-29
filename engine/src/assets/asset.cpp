#include "asset.hpp"
#include "resource_compiler.hpp"

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	casset::casset(stringview_t compiler_type, stringview_t source_path, stringview_t compiled_path, sasset_meta&& meta, rttr::variant&& options)
	{
		m_data.m_compiler_type.assign(compiler_type.data());
		m_data.m_source_path.assign(source_path.data());
		m_data.m_compiled_path.assign(compiled_path.data());
		m_data.m_meta = std::move(meta);
		m_data.m_options = std::move(options);
	}

	//------------------------------------------------------------------------------------------------------------------------
	casset::casset(stringview_t compiler_type, stringview_t source_path, stringview_t compiled_path)
	{
		m_data.m_compiler_type.assign(compiler_type.data());
		m_data.m_source_path.assign(source_path.data());
		m_data.m_compiled_path.assign(compiled_path.data());

		//- default options for compiler type
		if (auto options = detail::default_compiler_options(rttr::type::get_by_name(compiler_type.data())); options.is_valid())
		{
			m_data.m_options = options;
		}
	}

} //- engine