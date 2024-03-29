#include "slang.hpp"

namespace slang
{
	namespace
	{
		struct simpl
		{
			[[nodiscard]] sbytecode scan(std::string_view code);
		};

		sbytecode simpl::scan(std::string_view code)
		{
			sbytecode bytecode;

			for(const char& c : code)
			{
			
			}
		
			return std::move(bytecode);
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	sbytecode cscanner::scan(std::string_view code)
	{
		static simpl impl;
		
		return std::move(impl.scan(code));
	}
} //- slang