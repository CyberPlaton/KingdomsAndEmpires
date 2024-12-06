#pragma once
#include <core.h>
#include "../sm_config.hpp"
#include "sm_tiny_shading_language_lexer.hpp"

namespace sm
{
	namespace tsl
	{
		class itiny_shader_compiler;
		using tiny_shader_compiler_ref_t = ref_t<itiny_shader_compiler>;

		//------------------------------------------------------------------------------------------------------------------------
		enum compile_result : uint8_t
		{
			compile_result_ok = 0,
			compile_result_fail = 255,
		};

		namespace compiling
		{
			//- Configuration of token compilation.
			//------------------------------------------------------------------------------------------------------------------------
			struct sconfig final
			{

			};

		} //- compiling

		namespace detail
		{
			//------------------------------------------------------------------------------------------------------------------------
			class ccompiling_context final :
				private core::cnon_copyable,
				private core::cnon_movable
			{
			public:
				struct scursor
				{
					stoken m_token;
					unsigned m_current = 0;
					unsigned m_line = 0;
				};

				struct sdata
				{

				};

				ccompiling_context();
				~ccompiling_context() = default;

				void init(token_stream_t&& stream, compiling::sconfig cfg = {});

				inline compiling::sconfig& cfg() { return m_cfg; }

				stoken consume(token_type expect, const string_t& message);
				inline stoken advance() { return m_stream[++m_cursor.m_current]; }
				inline const stoken& peek(unsigned lookahead = 0) const { return m_stream[m_cursor.m_current + lookahead]; }
				inline bool at_end() const { return m_stream.end()->m_type == token_type_eof; }
				inline bool& result() { return m_result; }
				inline sdata& data() { return m_data; }
				inline scursor& cursor() { return m_cursor; }

				void emit_error(const string_t& message);

			private:
				sdata m_data;
				scursor m_cursor;
				token_stream_t m_stream;
				compiling::sconfig m_cfg;
				bool m_result;
			};

		} //- detail

		//- Interface class responsible for compiling tiny shader language to target shading language.
		//------------------------------------------------------------------------------------------------------------------------
		class itiny_shader_compiler
		{
		public:
			virtual compile_result	compile(stringview_t) = 0;
			virtual compile_result	compile(const memory_ref_t&) = 0;
			virtual string_t		emit() = 0;

		protected:
			detail::ccompiling_context m_ctx;
			string_t m_code;
		};

	} //- tsl

} //- sm