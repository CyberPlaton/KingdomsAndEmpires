#pragma once
#include <core.h>

namespace engine
{
	//- Interface class for any type of resource compilers that take in a source resource such as a 'png' file and
	//- transform it into a game/engine-ready format, i.e. a 'dds' or just copy data to destination folders.
	//------------------------------------------------------------------------------------------------------------------------
	class iresource_compiler
	{
	public:
		using compile_result_t = std::pair<bool, ref_t<core::cmemory>>;

		virtual compile_result_t compile(const ref_t<core::cmemory>& source_data, const rttr::variant& compiling_options) = 0;

		RTTR_ENABLE();
	};

	//- A compiler that copies source data as is to output folders.
	//------------------------------------------------------------------------------------------------------------------------
	class cdefault_compiler final : public iresource_compiler
	{
	public:
		struct soptions{};

		compile_result_t compile(const ref_t<core::cmemory>& source_data, const rttr::variant& compiling_options) override final;
	};

} //- engine