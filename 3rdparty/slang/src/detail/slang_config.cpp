#include "slang_config.hpp"

namespace slang
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		std::pair<bool, detail::sscope*> scope_find_variable(detail::sscope* scope, stringview_t name)
		{
			const auto& values = scope->m_values;

			if (values.find(name.data()) != values.end())
			{
				return { true, scope };
			}
			if (scope->m_parent)
			{
				return scope_find_variable(scope->m_parent.get(), name);
			}
			return { false, nullptr };
		}

	} //- unnamed

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		void slogger::init(slang_logger_t callback, log_level level /*= log_level_warn*/)
		{
			if (level > log_level_none)
			{
				m_log = std::move(callback);
				m_level = level;
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool sscope::lookup(stringview_t name)
		{
			return scope_find_variable(this, name).first;
		}

		//- Expect crash if we can not find the value
		//------------------------------------------------------------------------------------------------------------------------
		const svalue& sscope::at(stringview_t name) const
		{
			if (m_values.find(name.data()) != m_values.end())
			{
				return m_values.at(name.data());
			}
			return m_parent->at(name);
		}

		//------------------------------------------------------------------------------------------------------------------------
		svalue& sscope::edit(stringview_t name)
		{
			if (auto [result, scope] = scope_find_variable(this, name); result)
			{
				return scope->m_values[name.data()];
			}
			return m_values[name.data()];
		}

		//------------------------------------------------------------------------------------------------------------------------
		svalue& sscope::emplace(stringview_t name)
		{
			return m_values[name.data()];
		}

		//- line is the associated line in code associated with this op code, makes tracking and understanding of runtime
		//- errors easier
		//------------------------------------------------------------------------------------------------------------------------
		void schunk::write(byte_t byte, uint32_t line)
		{
			m_code.emplace_back(byte);
			m_lines.emplace_back(line);
		}

	} //- detail

} //- slang