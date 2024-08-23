#pragma once
#include "slang_types.hpp"

namespace slang
{
	namespace detail
	{
		struct scursor
		{
			string_t m_text;
			uint32_t m_current = 0;
			uint32_t m_line = 0;
		};

		//- Responsible for providing context and commonly used structures to parts of scanning code. Moreover it allows
		//- for customization and extensions through configuration.
		//------------------------------------------------------------------------------------------------------------------------
		class cscanning_context :
			private cnon_copyable,
			private cnon_movable
		{
		public:
			struct sconfig
			{

			};

			explicit cscanning_context(const char* code, sconfig cfg = {});
			~cscanning_context() = default;

			inline scursor& cursor() { return m_cursor; }
			inline stoken_stream& stream() { return m_stream; }
			inline [[nodiscard]] detail::stoken_stream&& take_stream() { return std::move(m_stream); }
			inline sconfig& cfg() { return m_cfg; }
			inline stringview_t& code() { return m_code; }
			inline compile_result& result() { return m_result; }

		private:
			scursor m_cursor;
			stoken_stream m_stream;
			sconfig m_cfg;
			stringview_t m_code;
			compile_result m_result;
		};

	} //- detail


	//- Responsible for generating a stream of tokens from raw source code. The resulting token stream is then further processed
	//- by the compiler class.
	//------------------------------------------------------------------------------------------------------------------------
	class cscanner final
	{
	public:
		explicit cscanner(const char* code, detail::cscanning_context::sconfig cfg = {});
		~cscanner() = default;

		bool scan();

		[[nodiscard]] detail::stoken_stream&& stream() { return m_ctx.take_stream(); }

	private:
		detail::cscanning_context m_ctx;
	};

} //- slang