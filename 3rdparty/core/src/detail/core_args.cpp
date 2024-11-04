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

} //- core