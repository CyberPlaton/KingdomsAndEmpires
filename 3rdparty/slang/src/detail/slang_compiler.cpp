#include "slang_compiler.hpp"

namespace slang
{
	namespace detail
	{
		namespace
		{
			static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
          c == '_';
}
//< is-alpha
//> is-digit
static bool isDigit(char c) {
  return c >= '0' && c <= '9';
}
//< is-digit
//> is-at-end
static bool isAtEnd() {
  return *scanner.current == '\0';
}
//< is-at-end
//> advance
static char advance() {
  scanner.current++;
  return scanner.current[-1];
}
//< advance
//> peek
static char peek() {
  return *scanner.current;
}
//< peek
//> peek-next
static char peekNext() {
  if (isAtEnd()) return '\0';
  return scanner.current[1];
}
//< peek-next
//> match
static bool match(char expected) {
  if (isAtEnd()) return false;
  if (*scanner.current != expected) return false;
  scanner.current++;
  return true;
}
//< match
//> make-token
static Token makeToken(TokenType type) {
  Token token;
  token.type = type;
  token.start = scanner.start;
  token.length = (int)(scanner.current - scanner.start);
  token.line = scanner.line;
  return token;
}
//< make-token
//> error-token
static Token errorToken(const char* message) {
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int)strlen(message);
  token.line = scanner.line;
  return token;
}
//< error-token
//> skip-whitespace
static void skipWhitespace() {
  for (;;) {
    char c = peek();
    switch (c) {
      case ' ':
      case '\r':
      case '\t':
        advance();
        break;
//> newline
      case '\n':
        scanner.line++;
        advance();
        break;
//< newline
//> comment
      case '/':
        if (peekNext() == '/') {
          // A comment goes until the end of the line.
          while (peek() != '\n' && !isAtEnd()) advance();
        } else {
          return;
        }
        break;
//< comment
      default:
        return;
    }
  }
}
		}; //- unnamed
		
		bool ccompiler::is_identifier(char c)
		{
			return ((c >= 'a' && c <= 'z') ||
					(c >= 'A' && c <= 'Z') ||
					 c == '_');
		}
		
		stoken ccompiler::make_identifier()
		{
			m_cursor.m_current.m_text.clear();
			
			auto c = peek();
			
			while(is_identifier(c))
			{
				m_cursor.m_current.m_text.append(c);
				
				c = advance();
			}
		
			//- identify keywords or return as identifier
			if()
			
		}
		
		//------------------------------------------------------------------------------------------------------------------------
		token_type ccompiler::check_keyword(unsigned start, const string_t& word, token_type type)
		{
			auto begin = m_code[m_cursor.m_current - start];

			if (std::memcmp(begin, word, m_cursor.m_current - start) == 0)
			{
				return type;
			}

			return token_type::token_type_identifier;
		}
		
		//------------------------------------------------------------------------------------------------------------------------
		detail::stoken ccompiler::next_token()
		{
			m_cursor.m_text.clear();

			auto c = peek();

			return {};
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ccompiler::process_token(const detail::stoken& token)
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		compile_result ccompiler::compile(stringview_t code)
		{
			m_code = code;

			for (auto i = 0; i < m_code.size(); ++i)
			{
				const auto& c = m_code[i];

				process_token(next_token());
			}

			return m_result;
		}
		
	} //- detail

} //- slang