#pragma once
#include "platform.hpp"
#include <string>
#include <cstdint>
#include <cassert>
#include <variant>
#include <stdarg.h>
#include <cstdio>
#include <cstring>

//- If overriding STL classes define the namespace they are coming from same as below
//------------------------------------------------------------------------------------------------------------------------
#ifndef SLANG_CUSTOM_STL
#include "3rdparty/TINYSTL/vector.h"
#include "3rdparty/TINYSTL/unordered_map.h"
#include "3rdparty/TINYSTL/string.h"
#include "3rdparty/TINYSTL/string_view.h"
namespace slang
{
	namespace stl = tinystl;
} //- slang
#endif

//- 
//------------------------------------------------------------------------------------------------------------------------
#ifndef SLANG_CUSTOM_ALLOCATOR
#include "3rdparty/dlmalloc/malloc.h"
#endif


#ifdef DEBUG
#define __ASSERT__(expression, message) assert(expression && message)
#define SLANG_ASSERT(expression, ...) __ASSERT__(expression, __VA_ARGS__)
#else
#define __ASSERT__(expression, message)
#define SLANG_ASSERT(expression, ...)
#endif

namespace slang
{
	typedef
	void* (*slang_malloc_t)(std::size_t);

	typedef
	void(*slang_free_t)(void*);

	typedef
	void* (*slang_calloc_t)(std::size_t, std::size_t);

	typedef
	void* (*slang_realloc_t)(void*, std::size_t);

	typedef
	void* (*slang_memalign_t)(std::size_t, std::size_t);

	typedef
	void* (*slang_valloc_t)(std::size_t);

	typedef
	void(*slang_logger_t)(uint8_t, const char*);

	struct sobject;

	namespace detail
	{
		struct sallocator;

	} //- detail


	template<typename T, typename TAllocator = detail::sallocator>
	using vector_t = stl::vector<T, TAllocator>;

	template<typename TKey, typename TValue, typename TAllocator = detail::sallocator>
	using umap_t = stl::unordered_map<TKey, TValue, TAllocator>;

	using string_t = stl::string;

	using stringview_t = stl::string_view;

	template<typename... TTypes>
	using variant_t = std::variant<TTypes...>;

	using byte_t = uint8_t;

	//- Result of compilation from source code to intermediate representation
	//------------------------------------------------------------------------------------------------------------------------
	enum compile_result : uint8_t
	{
		compile_result_ok = 0,
		compile_result_fail = 255,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum value_type : uint8_t
	{
		value_type_null = 0,
		value_type_integer,
		value_type_float,
		value_type_boolean,
		value_type_object,
	};

	//- Struct holding one of possible primitive value types
	//------------------------------------------------------------------------------------------------------------------------
	struct svalue
	{
		//------------------------------------------------------------------------------------------------------------------------
		template<typename TValue>
		inline static svalue make_value(TValue value, value_type type)
		{
			return { value, type };
		}

		template<typename TType>
		inline bool is() { return std::holds_alternative<TType>(as); }

		template<typename TType>
		inline TType& get() { return std::get<TType>(as); }

		template<typename TType>
		inline const TType& get() const { return std::get<TType>(as); }

		variant_t<int, float, bool, sobject*> as;
		value_type m_type;
	};

	namespace detail
	{
		//- Instructions to be executed by the VM
		//------------------------------------------------------------------------------------------------------------------------
		enum opcode : uint8_t
		{
			opcode_noop = 0,
			opcode_return,
			opcode_add,
			opcode_subtract,
			opcode_multiply,
			opcode_divide,

			opcode_constant,
			opcode_variable,
		};

		//------------------------------------------------------------------------------------------------------------------------
		enum token_type : uint8_t
		{
			token_type_none = 0,
			token_type_eof,
			token_type_error,

			token_type_identifier,		//- i.e. variable, function, class names etc.
			token_type_equal,			//- =
			token_type_exclamation,		//- !
			token_type_colon,			//- :
			token_type_semicolon,		//- ;
			token_type_comma,			//- ,
			token_type_dot,				//- .
			token_type_left_bracket,	//- [
			token_type_right_bracket,	//- ]
			token_type_left_brace,		//- {
			token_type_right_brace,		//- }
			token_type_left_paren,		//- (
			token_type_right_paren,		//- )

			//- keywords
			token_type_true,			//- true
			token_type_false,			//- false
			token_type_null,			//- null
			token_type_class,			//- class
			token_type_def,				//- def
			token_type_return,			//- return
		};

		//------------------------------------------------------------------------------------------------------------------------
		enum precedence_type : uint8_t
		{
			precedence_type_none = 0,
			precedence_type_assignment,	//- =
			precedence_type_or,			//- or
			precedence_type_and,		//- and
			precedence_type_equality,	//- == !=
			precedence_type_comparison,	//- < > <= >=
			precedence_type_term,		//- + -
			precedence_type_factor,		//- * /
			precedence_type_unary,		//- ! -
			precedence_type_call,		//- . ()
			precedence_type_primary,	//-
		};

		//------------------------------------------------------------------------------------------------------------------------
		enum log_level : int8_t
		{
			log_level_none = -1,
			log_level_trace = 0,
			log_level_debug,
			log_level_info,
			log_level_warn,
			log_level_error,
			log_level_critical,
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct sallocator
		{
			static void* malloc(std::size_t s);
			static void free(void* p, std::size_t /*bytes*/);
			static void* calloc(std::size_t n, std::size_t s);
			static void* realloc(void* p, std::size_t s);
			static void* memalign(std::size_t n, std::size_t s);
			static void* valloc(std::size_t s);

			void init();

			slang_malloc_t m_malloc		= nullptr;
			slang_free_t m_free			= nullptr;
			slang_calloc_t m_calloc		= nullptr;
			slang_realloc_t m_realloc	= nullptr;
			slang_memalign_t m_memalign = nullptr;
			slang_valloc_t m_valloc		= nullptr;
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct slogger
		{
			void init(slang_logger_t callback, log_level level = log_level_warn);

			slang_logger_t m_log = nullptr;
			log_level m_level = log_level_none;
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct stoken
		{
			uint32_t m_line = 0;
			stringview_t m_text;
			token_type m_type = token_type_null;
		};

		//- Code chunk, holding compiled instructions
		//------------------------------------------------------------------------------------------------------------------------
		struct schunk
		{
			vector_t<svalue> m_constants;
			vector_t<byte_t> m_code;
		};

	} //- detail

} //- slang