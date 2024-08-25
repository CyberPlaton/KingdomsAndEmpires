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
#include <functional>
#include <limits>
#include <array>
#define FMT_HEADER_ONLY
#include "3rdparty/fmt/format.h"
#include <rttr.h>

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

//------------------------------------------------------------------------------------------------------------------------
#ifndef SLANG_CUSTOM_ALLOCATOR
#include "3rdparty/dlmalloc/malloc.h"
	#define SLANG_MALLOC(s)			dlmalloc(s)
	#define SLANG_FREE(p)			dlfree(p)
	#define SLANG_CALLOC(n, size)	dlcalloc(n, size)
	#define SLANG_REALLOC(p, size)	dlrealloc(p, size)
	#define SLANG_MALLOCA(size, a)	dlmemalign(a, size)
	#define SLANG_FREEN(p, n)		SLANG_FREE(p)
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
#if TRACY_ENABLE
#include <Tracy.h>
#endif

//------------------------------------------------------------------------------------------------------------------------
#if PROFILE_ENABLE && TRACY_ENABLE
#define SLANG_CPU_ZONE ZoneScoped
#define SLANG_NAMED_CPU_ZONE(name) ZoneScopedN(name)
#elif PROFILE_ENABLE && SLANG_CUSTOM_PROFILE
//- SLANG_CPU_ZONE
//- SLANG_NAMED_CPU_ZONE(name)
#elif PROFILE_ENABLE
#define SLANG_CPU_ZONE
#define SLANG_NAMED_CPU_ZONE(name)
#else
#define SLANG_CPU_ZONE
#define SLANG_NAMED_CPU_ZONE(name)
#endif

//------------------------------------------------------------------------------------------------------------------------
#if CORE_PLATFORM_WINDOWS
	#define SLANG_NO_VTABLE __declspec(novtable)
#else
	#define SLANG_NO_VTABLE
#endif
#define STATIC_INSTANCE(__class) static __class& instance() { static __class S_INSTANCE; return S_INSTANCE; }
#define MAX(type) std::numeric_limits<type>().max()
#define MIN(type) std::numeric_limits<type>().min()
#define BIT(x) 1 << x

namespace slang
{
	typedef void(*slang_logger_t)(uint8_t, const char*);

	//- Class responsible for allocating and freeing memory. Function names are defines as malloc and free
	//- for compatability with tinystl.
	//------------------------------------------------------------------------------------------------------------------------
	class SLANG_NO_VTABLE iallocator
	{
	public:
		virtual void* malloc(size_t) = 0;
		virtual void free(void*, size_t) = 0;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class callocator final : public iallocator
	{
	public:
		void* malloc(size_t n) override final;
		void free(void* p, size_t n) override final;
	};

	template<typename T, typename TAllocator = callocator>
	using vector_t		= stl::vector<T, TAllocator>;
	template<typename TKey, typename TValue, typename TAllocator = callocator>
	using umap_t		= stl::unordered_map<TKey, TValue, TAllocator>;
	template<typename TType, size_t SIZE>
	using array_t		= std::array<TType, SIZE>;
	using string_t		= stl::basic_string<callocator>;
	using stringview_t	= stl::string_view;
	template<typename... TTypes>
	using variant_t		= std::variant<TTypes...>;
	template<typename T>
	using ptr_t			= std::unique_ptr<T>;
	template<typename T>
	using ref_t			= std::shared_ptr<T>;
	using byte_t		= uint8_t;

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
	enum object_type : uint8_t
	{
		object_type_none = 0,

		//- primitive object types
		object_type_null,
		object_type_number,
		object_type_boolean,

		//- composite or non-primitive object types
		object_type_string,
		object_type_function,
		object_type_struct
	};

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

		opcode_not,
		opcode_negate,
		opcode_equal,
		opcode_greater,
		opcode_smaller,
		opcode_smallerequal,
		opcode_greaterequal,

		opcode_variable,
		opcode_constant,
		opcode_true,
		opcode_false,
		opcode_null,
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
		token_type_not_equality,	//- !=
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

		token_type_count = token_type_return + 1,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum error_type : uint8_t
	{
		error_type_none = 0,
		error_type_unrecognized_literal,
		error_type_unterminated_string,
		error_type_multiline_string,


		error_type_missing_expected_eof,

		error_type_count = error_type_missing_expected_eof + 1,
	};

	//- Precedence ordered from lowest to highest
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
		precedence_type_primary,	//- STRING or NUMBER or "(" EXPRESSION ")" etc. for more see specs.slang Expressions
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

	namespace detail
	{
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

	} //- detail

	//- Forward declarations of types required by the state
	//------------------------------------------------------------------------------------------------------------------------
	namespace detail
	{
		class ccompiler;
		class cvm;

	} //- detail

	//- Constructor and Destructor are implemented in slang.cpp
	//- TODO: when we have an idea how to serialize runnable bytecode, we want to be able to load it from file
	//- and run without compilation.
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

} //- slang
