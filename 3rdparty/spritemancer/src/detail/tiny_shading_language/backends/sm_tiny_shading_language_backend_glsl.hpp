#pragma once
#include "../sm_tiny_shading_language_backend.hpp"

namespace sm
{
	//- Interface class responsible for compiling tiny shader language OpenGL GLSL.
	//------------------------------------------------------------------------------------------------------------------------
	class ctiny_shader_compiler_glsl final : public itiny_shader_compiler
	{
	public:
		compile_result	compile(stringview_t source) override final;
		compile_result	compile(const memory_ref_t& source) override final;
		string_t		emit() override final;

	private:
		std::string source;
		size_t position;

		// Helper: Skip whitespace
		void skipWhitespace() {
			while (position < source.size() && std::isspace(source[position])) {
				position++;
			}
		}

		// Helper: Match and consume a token
		bool match(const std::string& token) {
			skipWhitespace();
			if (source.compare(position, token.size(), token) == 0) {
				position += token.size();
				return true;
			}
			return false;
		}

		// Helper: Parse an identifier
		std::string parseIdentifier() {
			skipWhitespace();
			size_t start = position;
			while (position < source.size() && (std::isalnum(source[position]) || source[position] == '_')) {
				position++;
			}
			if (start == position) {
				throw std::runtime_error("Expected identifier at position: " + std::to_string(position));
			}
			return source.substr(start, position - start);
		}

		// <directive>
		void parseInclude() {
			skipWhitespace();
			if (!match("\"")) {
				throw std::runtime_error("Expected '\"' after #include at position: " + std::to_string(position));
			}
			size_t start = position;
			while (position < source.size() && source[position] != '"') {
				position++;
			}
			if (position == source.size()) {
				throw std::runtime_error("Unterminated string in #include at position: " + std::to_string(start));
			}
			std::string includedFile = source.substr(start, position - start);
			position++; // Consume closing '"'

			// Here, you could load the file and process it (not implemented in this function).
			std::cout << "Included file: " << includedFile << "\n";
		}

		// <function>
		Function parseFunction() {
			Function fn;
			fn.name = parseIdentifier();
			fn.parameters = parseParameters();
			if (!match("->")) {
				throw std::runtime_error("Expected '->' after function parameters at position: " + std::to_string(position));
			}
			fn.returnType = parseType();
			fn.body = parseBody();
			return fn;
		}

		// <parameters>
		std::vector<Parameter> parseParameters() {
			std::vector<Parameter> params;
			if (!match("(")) {
				throw std::runtime_error("Expected '(' at position: " + std::to_string(position));
			}

			while (!match(")")) {
				Parameter param;
				param.name = parseIdentifier();
				if (!match(":")) {
					throw std::runtime_error("Expected ':' in parameter declaration at position: " + std::to_string(position));
				}
				param.type = parseType();
				params.push_back(param);

				if (!match(",")) { // Optional comma
					break;
				}
			}

			return params;
		}

		// <type>
		std::string parseType() {
			return parseIdentifier(); // Types are identifiers (e.g., vec3, float)
		}

		// <body>
		std::string parseBody() {
			if (!match("{")) {
				throw std::runtime_error("Expected '{' at position: " + std::to_string(position));
			}

			size_t start = position;
			int braceCount = 1;
			while (position < source.size() && braceCount > 0) {
				if (source[position] == '{') {
					braceCount++;
				}
				else if (source[position] == '}') {
					braceCount--;
				}
				position++;
			}

			if (braceCount != 0) {
				throw std::runtime_error("Mismatched braces in function body");
			}

			return source.substr(start, position - start - 1); // Exclude the closing '}'
		}

		// <expression>
		std::string parseExpression() {
			std::string expr = parseTerm();
			skipWhitespace();

			while (position < source.size() && isOperator(source[position])) {
				char op = source[position++];
				expr += " ";
				expr += op;
				expr += " ";
				expr += parseTerm();
			}

			return expr;
		}

		// <term>
		std::string parseTerm() {
			skipWhitespace();
			if (std::isalnum(source[position]) || source[position] == '_') {
				return parseIdentifier();
			}
			else if (source[position] == '"') {
				return parseLiteral();
			}
			else if (source[position] == '(') {
				return parseFunctionCall();
			}
			else {
				throw std::runtime_error("Unexpected term at position: " + std::to_string(position));
			}
		}

		// <literal>
		std::string parseLiteral() {
			skipWhitespace();
			size_t start = position;
			if (std::isdigit(source[position]) || source[position] == '.') {
				while (std::isdigit(source[position]) || source[position] == '.') {
					position++;
				}
				return source.substr(start, position - start);
			}
			else {
				throw std::runtime_error("Unexpected literal at position: " + std::to_string(position));
			}
		}

		// <function_call>
		std::string parseFunctionCall() {
			std::string funcName = parseIdentifier();
			if (!match("(")) {
				throw std::runtime_error("Expected '(' in function call at position: " + std::to_string(position));
			}

			std::string args;
			while (!match(")")) {
				if (!args.empty()) args += ", ";
				args += parseExpression();
			}

			return funcName + "(" + args + ")";
		}

		// Check if the current character is an operator
		bool isOperator(char c) {
			return c == '+' || c == '-' || c == '*' || c == '/' || c == '%';
		}
	};

} //- sm