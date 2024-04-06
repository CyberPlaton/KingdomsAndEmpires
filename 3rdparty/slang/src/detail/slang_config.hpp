#pragma once
#include "platform.hpp"
#include <string>
#include <cstdint>
#include <cassert>
#include <variant>
#include <stdarg.h>
#include <cstdio>
#include <cstring>
#include <memory>

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

//- Note: to use custom allocators define SLANG_CUSTOM_ALLOCATOR, SLANG_MALLOC() and SLANG_FREE()
//------------------------------------------------------------------------------------------------------------------------
#ifndef SLANG_CUSTOM_ALLOCATOR
#include "3rdparty/dlmalloc/malloc.h"
	//- TODO: reconsider memory management and allocation
	#define SLANG_MALLOC(s)	dlmalloc(s)
	#define SLANG_FREE(p)	dlfree(p)
#endif

//- @reference: imgui_inernal.h 303 or https://github.com/scottt/debugbreak
//------------------------------------------------------------------------------------------------------------------------
#ifndef SLANG_CUSTOM_DEBUGGER
	#if defined (_MSC_VER)
		#define SLANG_DEBUG_BREAK() __debugbreak()
	#elif defined(__clang__)
		#define SLANG_DEBUG_BREAK() __builtin_debugtrap()
	#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
		#define SLANG_DEBUG_BREAK() __asm__ volatile("int $0x03")
	#elif defined(__GNUC__) && defined(__thumb__)
		#define SLANG_DEBUG_BREAK() __asm__ volatile(".inst 0xde01")
	#elif defined(__GNUC__) && defined(__arm__) && !defined(__thumb__)
		#define SLANG_DEBUG_BREAK() __asm__ volatile(".inst 0xe7f001f0");
	#else
		//- It is expected that you define SLANG_DEBUG_BREAK()!
		#define SLANG_DEBUG_BREAK() IM_ASSERT(0)
	#endif
#endif

//------------------------------------------------------------------------------------------------------------------------
#ifdef DEBUG
#define __ASSERT__(expression, message) assert(expression && message)
#define SLANG_ASSERT(expression, ...) __ASSERT__(expression, __VA_ARGS__)
#else
#define __ASSERT__(expression, message)
#define SLANG_ASSERT(expression, ...)
#endif

//------------------------------------------------------------------------------------------------------------------------
#ifndef SLANG_NO_TRACY
#include <Tracy.h>
#endif

namespace slang
{
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

	using string_t = stl::basic_string<detail::sallocator>;

	using stringview_t = stl::string_view;

	template<typename... TTypes>
	using variant_t = std::variant<TTypes...>;

	template<typename T>
	using ptr_t = std::unique_ptr<T>;

	template<typename T>
	using ref_t = std::shared_ptr<T>;

	using byte_t = uint8_t;

	//- Result of compilation from source code to intermediate representation
	//------------------------------------------------------------------------------------------------------------------------
	enum compile_result : uint8_t
	{
		compile_result_ok = 0,
		compile_result_fail = 255,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum interpret_result : uint8_t
	{
		interpret_result_ok = 0,
		interpret_result_fail = 255,
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

	//------------------------------------------------------------------------------------------------------------------------
	enum object_type : uint8_t
	{
		object_type_none = 0,
		object_type_null,
		object_type_string,
		object_type_function,
		object_type_struct
	};

	//- Struct holding one of possible primitive value types
	//------------------------------------------------------------------------------------------------------------------------
	struct svalue
	{
		template<typename TValue>
		static svalue create(TValue value, value_type type);

		template<typename TType>
		bool is() const;

		template<typename TType>
		TType& get();

		template<typename TType>
		const TType& get() const;

		variant_t<int, float, bool, sobject*> as;
		value_type m_type;
	};

	//- Base of a language construct
	//------------------------------------------------------------------------------------------------------------------------
	struct sobject
	{
		sobject() : m_marked(false), m_type(object_type_none) {}
		sobject(object_type type) : m_marked(false), m_type(type) {}

		template<typename TObject>
		TObject* as();

		template<typename TObject>
		const TObject* as() const;

		bool m_marked;
		object_type m_type;
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
			token_type_number,			//- i.e. 1.2, 0, 0.25
			token_type_string,			//- i.e. "Hello World"
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

			token_type_minus,			//- -
			token_type_plus,			//- +
			token_type_star,			//- *
			token_type_slash,			//- /

			token_type_smaller,			//- <
			token_type_greater,			//- >
			token_type_equality,		//- ==
			token_type_smaller_equal,	//- <=
			token_type_greater_equal,	//- >=

			//- keywords
			token_type_true,			//- true
			token_type_false,			//- false
			token_type_null,			//- null
			token_type_class,			//- class
			token_type_def,				//- def
			token_type_var,				//- var (variable declaration)
			token_type_this,			//- this
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

		//- Note: when creating objects make sure to use static_new and static_delete,
		//- otherwise their constructor/destructor will not be invoked
		//------------------------------------------------------------------------------------------------------------------------
		struct sallocator
		{
			static void* malloc(std::size_t s);
			static void free(void* p, std::size_t /*bytes*/);
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
			string_t m_text;
			token_type m_type = token_type_null;
		};

		//- Sequenced array of tokens, in order of their encounter or declaration
		//------------------------------------------------------------------------------------------------------------------------
		struct stoken_stream
		{
			vector_t<stoken> m_stream;
		};

		//- Scoped variables. Mapped to their declared names.
		//------------------------------------------------------------------------------------------------------------------------
		struct sscope
		{
			//- Is variable declared in current or upward scope
			bool lookup(stringview_t name);

			//- Constant access to variable
			const svalue& at(stringview_t name) const;

			//- Get reference to variable if present in current or upward scope, otherwise emplace in current
			svalue& edit(stringview_t name);

			//- Insert variable in current scope
			svalue& emplace(stringview_t name);

			umap_t<string_t, svalue> m_values;
			ref_t<sscope> m_parent = nullptr;
		};

		//- Code chunk, holding compiled instructions and his constants. Basically the intermediate representation for slang
		//------------------------------------------------------------------------------------------------------------------------
		struct schunk
		{
			vector_t<svalue> m_constants;
			vector_t<byte_t> m_code;
		};

	} //- detail

	//- Forward declarations of types required by the state
	//------------------------------------------------------------------------------------------------------------------------
	namespace detail
	{
		class ccompiler;
		class cvm;

	} //- detail

	//- Constructor and Destructor are implemented in slang.cpp
	//------------------------------------------------------------------------------------------------------------------------
	class cslang_state final
	{
	public:
		cslang_state();
		~cslang_state();

		//- compile to IR without executing the code
		compile_result compile(stringview_t code);

		//- execute compiled IR in order of definition
		interpret_result run();

	private:
		ptr_t<detail::ccompiler> m_compiler;
		ptr_t<detail::cvm> m_vm;
	};


	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	const TType& svalue::get() const
	{
		SLANG_ASSERT(!as.valueless_by_exception(), "Invalid operation. Value does not hold any data");
		SLANG_ASSERT(is<TType>(), "Invalid operation. Value does not have requested type");
		return std::get<TType>(as);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	TType& svalue::get()
	{
		SLANG_ASSERT(!as.valueless_by_exception(), "Invalid operation. Value does not hold any data");
		SLANG_ASSERT(is<TType>(), "Invalid operation. Value does not have requested type");
		return std::get<TType>(as);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	bool svalue::is() const
	{
		return std::holds_alternative<TType>(as);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TValue>
	svalue svalue::create(TValue value, value_type type)
	{
		return { value, type };
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TObject>
	TObject* sobject::as()
	{
		return static_cast<TObject*>(this);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TObject>
	const TObject* sobject::as() const
	{
		return static_cast<const TObject*>(this);
	}

} //- slang