#pragma once
#include "slang_types.hpp"
#include "slang_object.hpp"

namespace slang
{
	namespace detail
	{
		//- Scanner and bytecode generator. Takes code text and creates an intermediate representation ready to be run on a VM
		//------------------------------------------------------------------------------------------------------------------------
		class ccompiler
		{
		public:
			ccompiler() = default;
			~ccompiler() = default;

			compile_result compile(stringview_t code);

		private:
		};

	} //- detail

} //- slang