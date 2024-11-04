#include "core_args.hpp"

namespace core
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		struct sconstructor_enabler final : public cargs
		{
			sconstructor_enabler(stringview_t name, stringview_t version) : cargs(name, version)
			{
			}
		};

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	args_ref_t cargs::make_ref(stringview_t name /*= ""*/, stringview_t version /*= ""*/)
	{
		return std::make_shared<sconstructor_enabler>(name, version);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cargs::cargs(stringview_t name /*= ""*/, stringview_t version /*= ""*/) :
		m_parser(name.data(), version.data())
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cargs::used(stringview_t name) const
	{
		return m_parser.is_used(name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cargs& cargs::flag(stringview_t name, stringview_t short_name, const bool default_value /*= false*/)
	{
		m_parser
			.add_argument(short_name.data(), name.data())
			.default_value(default_value)
			.implicit_value(true);

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cargs::parse(int argc, char* argv[])
	{
		try
		{
			if (argc > 0 && argv != nullptr)
			{
				m_parser.parse_args(argc, argv);
			}
		}
		catch (...)
		{
			return false;
		}

		return true;
	}

} //- core