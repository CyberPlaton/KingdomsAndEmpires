#pragma once
#include <core.h>
#include "../detail/tree_sitter_integration/tree_sitter_integration.h"

namespace sm
{
	namespace textedit
	{
		using coordinate_t = TSPoint;

		//-------------------------------------------------------------------------------------------------------
		enum language_type : uint8_t
		{
			language_type_none = 0,
			language_type_lua,
		};

		//-------------------------------------------------------------------------------------------------------
		enum highlight_token : uint8_t
		{
			highlight_token_none = 0,
			highlight_token_normal,
			highlight_token_comment,
			highlight_token_multiline_comment,
			highlight_token_string,
			highlight_token_number,
			highlight_token_match,
			highlight_token_function,
			highlight_token_constant,
			highlight_token_field,
			highlight_token_primary_keyword,
			highlight_token_secondary_keyword,
			highlight_token_primary_special_character,
			highlight_token_secondary_special_character,
		};

		//- Containing one row of source code.
		//- Note that text length is same size as length of highlight vector,
		//- because the highlight vector defines the highlight value for each character in text.
		//-------------------------------------------------------------------------------------------------------
		struct srow
		{
			string_t m_text;
			vector_t<highlight_token> m_highlight;
		};

		//- Stores information on how to highlight a particular filetype.
		//-------------------------------------------------------------------------------------------------------
		struct ssyntax
		{
			static ssyntax syntax(language_type lang);

			vector_t<string_t> m_file_extensions;
			vector_t<string_t> m_primary_keywords;
			vector_t<string_t> m_secondary_keywords;
			vector_t<string_t> m_primary_special_characters;
			vector_t<string_t> m_secondary_special_characters;

			TSTree* m_tree = nullptr;			//- syntax tree
			TSLanguage* m_language = nullptr;	//- current language
			TSParser* m_parser = nullptr;		//- parser for current language

			language_type m_language_type = language_type_none;
		};

		//- Backend of a text editor. Responsible for parsing and highlighting of the source code, checking
		//- the syntax and updating, performing actions such as undo or redo etc.
		//-------------------------------------------------------------------------------------------------------
		class cbackend final
		{
		public:
			cbackend();
			~cbackend();

			bool init(language_type lang, const string_t& source);
			void shutdown();
			void update();

			const srow& row_at(unsigned i) const;
			srow& row_at(unsigned i);

			string_t current_source() const;
			unsigned row_count() const;

		private:
			ssyntax m_syntax;
			vector_t<srow> m_source;

			void text_to_rows(const string_t& text);
			string_t rows_to_text() const;
			void highlight(unsigned start_row, unsigned end_row);
			void highlight_subtree(TSNode root, unsigned start_row, unsigned end_row);
			void highlight_rows(coordinate_t start, coordinate_t end, highlight_token highlight);
		};

	} //- textedit

} //- sm