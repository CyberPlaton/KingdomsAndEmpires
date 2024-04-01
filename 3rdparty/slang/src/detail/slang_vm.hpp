#pragma once
#include "slang_types.hpp"
#include "slang_object.hpp"

namespace slang
{
	namespace detail
	{
		//- Virtual machine, takes IR code and executes it
		//------------------------------------------------------------------------------------------------------------------------
		class cvm
		{
		public:
			cvm() = default;
			~cvm() = default;

		private:
		};

	} //- detail

} //- slang