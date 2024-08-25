#pragma once
#include "slang_config.hpp"
#include "slang_value.hpp"

namespace slang
{
	//- Responsible for storing declared constant values and bytecode instructions intended for a scope.
	//------------------------------------------------------------------------------------------------------------------------
	class cchunk final
	{
	public:
		cchunk();
		~cchunk() = default;

		void write_opcode(byte_t byte, unsigned line);
		template<typename T>
		void write_value(object_type type, T value);
		void write_value(cvalue&& value);

	private:
		vector_t<cvalue> m_constants;
		vector_t<byte_t> m_code;
		vector_t<unsigned> m_lines;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void cchunk::write_value(object_type type, T value)
	{
		m_constants.emplace_back(std::move(cvalue(type, value)));
	}

} //- slang