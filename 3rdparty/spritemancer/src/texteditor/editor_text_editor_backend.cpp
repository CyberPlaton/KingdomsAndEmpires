#include "editor_text_editor_backend.hpp"

namespace sm
{
	namespace textedit
	{
		namespace
		{
			constexpr array_t<stringview_t, 4> C_NUMBERS = { "number_literal", "integer", "float", "integer_literal" };
			constexpr array_t<stringview_t, 2> C_TYPES = { "storage_class_specifier", "type_qualifier" };
			constexpr array_t<stringview_t, 3> C_PRIMITIVES = { "primitive_type", "type_identifier", "sized_type_specifier" };
			constexpr array_t<stringview_t, 5> C_STRING = { "string_literal", "system_lib_string", "char_literal", "string", "raw_string_literal" };
			constexpr array_t<stringview_t, 4> C_ESCAPES = { "number_literal", "integer", "float", "integer_literal" };
			constexpr array_t<stringview_t, 2> C_COMMENTS = { "comment", "line_comment" };
			constexpr array_t<stringview_t, 4> C_UNARIES = { "unary_expression", "pointer_expression", "pointer_declarator", "abstract_pointer_declarator" };
			constexpr array_t<stringview_t, 4> C_FUNCTIONS = { "function_declarator", "call_expression", "call", "function_item" };

			//-------------------------------------------------------------------------------------------------------
			template<typename TIterator>
			bool is_one_of(const char* string, TIterator begin, TIterator end)
			{
				return algorithm::find(begin, end, string);
			}

		} //- unnamed

		//-------------------------------------------------------------------------------------------------------
		bool cbackend::init(language_type lang)
		{
			m_syntax = ssyntax::syntax(lang);

			return true;
		}

		//-------------------------------------------------------------------------------------------------------
		void cbackend::shutdown()
		{

		}

		//-------------------------------------------------------------------------------------------------------
		void cbackend::update()
		{

		}

		//-------------------------------------------------------------------------------------------------------
		void cbackend::highlight(unsigned start_row, unsigned end_row)
		{
			auto root = ts_tree_root_node(m_syntax.m_tree);

			highlight_subtree(root, start_row, end_row);
		}

		//- This function is very important and should be refactored to be as fast as possible
		//-------------------------------------------------------------------------------------------------------
		void cbackend::highlight_subtree(TSNode root, unsigned start_row, unsigned end_row)
		{
			if (ts_node_is_null(root))
			{
				return;
			}

			const auto* type = ts_node_type(root);
			unsigned children = ts_node_child_count(root);

			coordinate_t start = ts_node_start_point(root);
			coordinate_t end = ts_node_end_point(root);

			highlight_token highlight = highlight_token_normal;

			//- language agnostic highlighting

			//- direct
			if (is_one_of(type, C_NUMBERS.begin(), C_NUMBERS.end()))
			{
				highlight = highlight_token_number;
			}
			else if (is_one_of(type, C_TYPES.begin(), C_TYPES.end()))
			{
				highlight = highlight_token_secondary_keyword;
			}
			else if (is_one_of(type, m_syntax.m_primary_keywords.begin(), m_syntax.m_primary_keywords.end()))
			{
				highlight = highlight_token_primary_keyword;
			}
			else if (is_one_of(type, m_syntax.m_secondary_keywords.begin(), m_syntax.m_secondary_keywords.end()))
			{
				highlight = highlight_token_secondary_keyword;
			}
			else if (is_one_of(type, m_syntax.m_primary_special_characters.begin(), m_syntax.m_primary_special_characters.end()))
			{
				highlight = highlight_token_primary_special_character;
			}
			else if (is_one_of(type, m_syntax.m_secondary_special_characters.begin(), m_syntax.m_secondary_special_characters.end()))
			{
				highlight = highlight_token_secondary_special_character;
			}
			else if (is_one_of(type, C_STRING.begin(), C_STRING.end()))
			{
				highlight = highlight_token_string;
			}
			else if (is_one_of(type, C_COMMENTS.begin(), C_COMMENTS.end()))
			{
				highlight = highlight_token_comment;
			}
			else if (is_one_of(type, C_PRIMITIVES.begin(), C_PRIMITIVES.end()))
			{
				highlight = highlight_token_secondary_keyword;
			}
			else if (is_one_of(type, C_ESCAPES.begin(), C_ESCAPES.end()))
			{
				highlight = highlight_token_primary_special_character;
			}
			else if (is_one_of(type, C_UNARIES.begin(), C_UNARIES.end()))
			{
				highlight = highlight_token_primary_keyword;

				end.column = start.column + 1;
			}
			else if (core::string_utils::compare(type, "escape_sequence"))
			{
				highlight = highlight_token_primary_special_character;
			}
			else if (!core::string_utils::compare(type, "null"))
			{
				highlight = highlight_token_secondary_special_character;
			}
			//- node child
			else if (!core::string_utils::compare(type, "return_statement"))
			{
				auto return_child = ts_node_child(root, 0);

				if (!ts_node_is_null(return_child))
				{
					highlight = highlight_token_secondary_keyword;
					start = ts_node_start_point(return_child);
					end = ts_node_end_point(return_child);
				}
			}
			//- complex
			//- identifiers
			else if (!core::string_utils::compare(type, "identifier"))
			{

			}
			//- identifiers part of a special keyword (?)
			//- function name
			else if (is_one_of(type, C_FUNCTIONS.begin(), C_FUNCTIONS.end()))
			{
				TSNode function_name;
				bool set = false;

				if (!core::string_utils::compare(type, "function_declarator"))
				{

				}
				else if (!core::string_utils::compare(type, "call_expression"))
				{
				}
				else if (!core::string_utils::compare(type, "call"))
				{
				}
				else if (!core::string_utils::compare(type, "function_item"))
				{
				}

				if (set && !ts_node_is_null(function_name))
				{
					highlight = highlight_token_function;
					start = ts_node_start_point(function_name);
					end = ts_node_end_point(function_name);
				}
			}

			//- language specific highlighting (cpp)

			//- fields

			//- ...

			//- proceed highlighting node childre
			for (auto i = 0u; i < children; ++i)
			{
				auto child = ts_node_child(root, i);

				highlight_subtree(child, start_row, end_row);
			}
		}

		//-------------------------------------------------------------------------------------------------------
		ssyntax ssyntax::syntax(language_type lang)
		{
			switch (lang)
			{
			case language_type_lua:
			{
				break;
			}
			case language_type_cpp:
			{
				ssyntax s;

				s.m_file_extensions = { ".c", ".h", ".cc", ".cpp" };
				s.m_primary_keywords = { "break", "case", "class", "continue", "do", "else", "enum", "false",
					"for", "goto", "if", "return", "sizeof", "struct", "switch", "true", "typedef",
					"union", "while" };
				s.m_secondary_keywords = { "#include", "#ifdef", "#endif", "#ifndef", "#define" };
				s.m_primary_special_characters = { "(", ")", "{", "}", "[", "]", ";", "." };
				s.m_secondary_special_characters = { "?", ":" };

				s.m_parser = ts_parser_new();
				s.m_language = tree_sitter_cpp();
				ts_parser_set_language(s.m_parser, s.m_language);

				//- Note: TSTree will be set on first parsing of source string
				return s;
			}
			default:
				break;
			}
			return {};
		}

	} //- textedit

} //- sm