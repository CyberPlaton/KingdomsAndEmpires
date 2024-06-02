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
		cbackend::cbackend()
		{
		}

		//-------------------------------------------------------------------------------------------------------
		cbackend::~cbackend()
		{
		}

		//-------------------------------------------------------------------------------------------------------
		bool cbackend::init(language_type lang, const string_t& source)
		{
			m_syntax = ssyntax::syntax(lang);

			text_to_rows(source);

			//- initial parsing of source code
			m_syntax.m_tree = ts_parser_parse_string(m_syntax.m_parser, nullptr, source.data(), source.length());

			return true;
		}

		//-------------------------------------------------------------------------------------------------------
		void cbackend::shutdown()
		{

		}

		//-------------------------------------------------------------------------------------------------------
		void cbackend::update()
		{
			//- If source has changed, re-highlight it
			highlight(0, m_source.size());
		}

		//-------------------------------------------------------------------------------------------------------
		void cbackend::text_to_rows(const string_t& text)
		{
			std::vector<string_t> rows;

			core::string_utils::split(text, '\n', rows);

			for (auto& row : rows)
			{
				auto& source_row = m_source.emplace_back();

				const auto length = row.length();

				//- init and set default highlight
				source_row.m_highlight.resize(length);
				std::memset(&source_row.m_highlight[0], highlight_token_normal, length);

				source_row.m_text.swap(row);
			}
		}

		//-------------------------------------------------------------------------------------------------------
		string_t cbackend::rows_to_text() const
		{
			string_t out;

			//- basically reverse split and contatenating
			for (const auto& row : m_source)
			{
				fmt::format("{}{}\n", out.data(), row.m_text.data());
			}

			return out;
		}

		//-------------------------------------------------------------------------------------------------------
		srow& cbackend::row_at(unsigned i)
		{
			return m_source[i];
		}

		//-------------------------------------------------------------------------------------------------------
		const srow& cbackend::row_at(unsigned i) const
		{
			return m_source.at(i);
		}

		//-------------------------------------------------------------------------------------------------------
		string_t cbackend::current_source() const
		{
			return rows_to_text();
		}

		//-------------------------------------------------------------------------------------------------------
		unsigned cbackend::row_count() const
		{
			return static_cast<unsigned>(m_source.size());
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
			else if (core::string_utils::compare(type, "null"))
			{
				highlight = highlight_token_secondary_special_character;
			}
			//- node child
			else if (core::string_utils::compare(type, "return_statement"))
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
			else if (core::string_utils::compare(type, "identifier"))
			{
				const auto length = end.column - start.column;

				//- constants
				if (length > 1)
				{
					auto& row = row_at(start.row);

					auto is_constant = true;
					for (auto i = start.column; i < end.column; ++i)
					{
						auto c = row.m_text[i];

						//- a constant consists of alpha numerics chars and separation symbol '_'
						if (!core::string_utils::is_alpha_numeric(c) && c != '_')
						{
							is_constant = false;
							break;
						}
					}

					if (is_constant)
					{
						highlight = highlight_token_constant;
					}
				}

				//- identifiers part of a special keyword
				//- TODO:
			}
			//- function name
			else if (is_one_of(type, C_FUNCTIONS.begin(), C_FUNCTIONS.end()))
			{
				TSNode function_name;
				bool set = false;

				if (core::string_utils::compare(type, "function_declarator"))
				{
					const auto* field = "declarator";
					function_name = ts_node_child_by_field_name(root, field, strlen(field));
					set = true;
				}
				else if (core::string_utils::compare(type, "call_expression"))
				{
					const auto* field = "function";
					TSNode function_child = ts_node_child_by_field_name(root, field, strlen(field));

					//- identifier with field 'function' is the name of the function
					if (!ts_node_is_null(function_child) && ts_node_child_count(function_child) == 0)
					{
						function_name = function_child;
					}
					//- function is part of a 'path', we need the 'name' field
					else
					{
						field = "name";
						function_name = ts_node_child_by_field_name(function_child, field, strlen(field));
					}

					set = true;
				}
				else if (core::string_utils::compare(type, "call"))
				{
					const auto* field = "function";
					TSNode function_child = ts_node_child_by_field_name(root, field, strlen(field));

					if (!ts_node_is_null(function_child))
					{
						const auto child_count = ts_node_child_count(function_child);

						//- identifier with field 'function' is the name of the function
						if (child_count == 0)
						{
							function_name = function_child;
						}
						//- function is part of an 'object', we need the 'attribute' field
						else
						{
							field = "attribute";
							function_name = ts_node_child_by_field_name(function_child, field, strlen(field));
						}

						set = true;
					}
				}
				else if (core::string_utils::compare(type, "function_item"))
				{
					const auto* field = "name";
					function_name = ts_node_child_by_field_name(root, field, strlen(field));
					set = true;
				}

				if (set && !ts_node_is_null(function_name))
				{
					highlight = highlight_token_function;
					start = ts_node_start_point(function_name);
					end = ts_node_end_point(function_name);
				}
			}

			//- language specific highlighting (cpp)
			switch (m_syntax.m_language_type)
			{
			case language_type_lua:
			{
				break;
			}
			case language_type_cpp:
			{
				//- fields
				if (core::string_utils::compare(type, "field_identifier"))
				{
					highlight = highlight_token_field;
				}
				//- ...
				break;
			}
			default:
				break;
			}

			//- set highlight
			if (highlight != highlight_token_normal)
			{
				//- validate that rows to be highlighted are within bounds specified
				//- by function parameter
				if (start.row >= start_row && start.row <= end_row &&
					end.row >= start_row && end.row <= end_row)
				{
					highlight_rows(start, end, highlight);
				}
			}

			//- proceed highlighting node childre
			for (auto i = 0u; i < children; ++i)
			{
				auto child = ts_node_child(root, i);

				highlight_subtree(child, start_row, end_row);
			}
		}

		//-------------------------------------------------------------------------------------------------------
		void cbackend::highlight_rows(coordinate_t start, coordinate_t end, highlight_token highlight)
		{
			auto& row = row_at(start.row);

			//- node spans multiple lines
			if (end.row > start.row)
			{
				//- set highlight for contiguous characters ranging from column start to
				//- column end
				std::memset(&row.m_highlight[start.column], highlight, row.m_text.length() - start.column);

				//- set highlight for complete row
				for (auto i = start.row + 1; i < end.row; ++i)
				{
					row = row_at(i);

					std::memset(&row.m_highlight[0], highlight, row.m_text.length());
				}

				row = row_at(end.row);

				std::memset(&row.m_highlight[0], highlight, end.column);
			}
			//- node spans single line
			else
			{
				std::memset(&row.m_highlight[start.column], highlight, end.column - start.column);
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