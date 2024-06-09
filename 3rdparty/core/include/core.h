#pragma once
#include <core/platform.hpp>
#if defined(CORE_USE_EASTL)
//- Note: if you decide to use EASTL, make sure that you are ready
//- to integrate all of them used structures to RTTR
#include <eastl.h>
namespace stl = eastl;
#else
#include <vector>
#include <array>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <deque>
#include <bitset>
namespace stl = std;
#endif
#include <new>
#include <glm.h>
#include <magic_enum.h>
#include <taskflow.h>
#include <spdlog.h>
#include <../src/rttr.hpp>
#include <nlohmann.h>
#include <../src/simdjson.h>
#include <../src/tracy.hpp>
#include <cstddef>
#include <memory>
#include <filesystem>
#include <random>
#include <cassert>
#include <future>
#include <chrono>
#include <any>
#include <fstream>
#include <string>

//------------------------------------------------------------------------------------------------------------------------
#if CORE_COMPILER_MSVC
#define CORE_LIKELY(x) x
#define CORE_UNLIKELY(x) x
#elif CORE_COMPILER_GCC || CORE_COMPILER_CLANG
#define CORE_LIKELY(x) __builtin_expect(!!(x), 1)
#define CORE_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define CORE_LIKELY(x) x
#define CORE_UNLIKELY(x) x
#endif

//------------------------------------------------------------------------------------------------------------------------
#define STRINGIFY(s) #s
#define STRING(s) STRINGIFY(s)
#define SSTRING(s) STRING(s)

//------------------------------------------------------------------------------------------------------------------------
#define MAX(type) std::numeric_limits<type>().max()
#define MIN(type) std::numeric_limits<type>().min()
#define BIT(x) 1 << x

//------------------------------------------------------------------------------------------------------------------------
#ifdef DEBUG
#define CORE_ASSERT(expression, message) assert(expression && message)
#define ASSERT(expression, ...) CORE_ASSERT(expression, __VA_ARGS__)
#else
#define CORE_ASSERT(expression, message)
#define ASSERT(expression, ...)
#endif

//- @reference: imgui_inernal.h 303 or https://github.com/scottt/debugbreak
//------------------------------------------------------------------------------------------------------------------------
#if DEBUG
#if defined (_MSC_VER)
#define CORE_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__)
#define CORE_DEBUG_BREAK() __builtin_debugtrap()
#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
#define CORE_DEBUG_BREAK() __asm__ volatile("int $0x03")
#elif defined(__GNUC__) && defined(__thumb__)
#define CORE_DEBUG_BREAK() __asm__ volatile(".inst 0xde01")
#elif defined(__GNUC__) && defined(__arm__) && !defined(__thumb__)
#define CORE_DEBUG_BREAK() __asm__ volatile(".inst 0xe7f001f0");
#else
	//- It is expected that you define CORE_DEBUG_BREAK()!
#define CORE_DEBUG_BREAK() CORE_ASSERT(0, "")
#endif
#endif

//- Required for allocator
//------------------------------------------------------------------------------------------------------------------------
#if CORE_COMPILER_MSVC
#define CORE_NO_VTABLE __declspec(novtable)
#elif CORE_COMPILER_GCC || CORE_COMPILER_CLANG
#define CORE_NO_VTABLE
#endif

//------------------------------------------------------------------------------------------------------------------------
#define SCAST(type, value) static_cast<type>(value)
#define RCAST(type, value) reinterpret_cast<type>(value)
#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif
#define ARRAYSIZE(__array) ((u32)(sizeof(__array) / sizeof(*(__array))))
#define STATIC_INSTANCE(__class, __member) static __class& instance() { static __class __member; return __member; }
#define STATIC_INSTANCE_EX(__class) STATIC_INSTANCE(__class, s_instance)

//------------------------------------------------------------------------------------------------------------------------
#if CORE_PLATFORM_WINDOWS && TRACY_ENABLE
	//------------------------------------------------------------------------------------------------------------------------
	inline static void* tracy_malloc_trace(std::size_t size)
	{
		void* p = std::malloc(size);

		TracyAlloc(p, size);

		return p;
	}

	//------------------------------------------------------------------------------------------------------------------------
	inline static void* tracy_calloc_trace(std::size_t n, std::size_t size)
	{
		void* p = std::calloc(n, size);

		TracyAlloc(p, n * size);

		return p;
	}

	//------------------------------------------------------------------------------------------------------------------------
	inline static void* tracy_realloc_trace(void* ptr, std::size_t size)
	{
		TracyFree(ptr);

		void* p = std::realloc(ptr, size);

		TracyAlloc(p, size);

		return p;
	}

	//------------------------------------------------------------------------------------------------------------------------
	inline static void tracy_free_trace(void* ptr)
	{
		TracyFree(ptr);
		std::free(ptr);
	}

	//------------------------------------------------------------------------------------------------------------------------
	#define CORE_MALLOC(size)		tracy_malloc_trace(size)
	#define CORE_CALLOC(n, size)	tracy_calloc_trace(n, size)
	#define CORE_REALLOC(p, size)	tracy_realloc_trace(p, size)
	#define CORE_FREE(p)			tracy_free_trace(p)
	#define CORE_FREEN(p, n)		tracy_free_trace(p)
#else
	//------------------------------------------------------------------------------------------------------------------------
	#define CORE_MALLOC(size)		std::malloc(size)
	#define CORE_CALLOC(n, size)	std::calloc(n, size)
	#define CORE_REALLOC(p, size)	std::realloc(p, size)
	#define CORE_FREE(p)			std::free(p)
	#define CORE_FREEN(p, n)		CORE_FREE(p)
#endif

//------------------------------------------------------------------------------------------------------------------------
#if defined(core_EXPORTS)
	#if CORE_PLATFORM_WINDOWS && TRACY_ENABLE
		void* operator new(unsigned long long n) { return CORE_MALLOC(n); }
		void operator delete(void* p) { CORE_FREE(p); }
	#endif
#endif

//------------------------------------------------------------------------------------------------------------------------
#if CORE_PLATFORM_WINDOWS && TRACY_ENABLE
#define CORE_ZONE ZoneScoped
#define CORE_NAMED_ZONE(name) ZoneScopedN(name)
#else
#define CORE_ZONE
#define CORE_NAMED_ZONE(name)
#endif

namespace core
{
	class iallocator;
	class clinear_allocator;
	class cstring;
	class cstringview;

} //- core

template<class T>
using ref_t = std::shared_ptr<T>;
template<class T>
using ptr_t = std::unique_ptr<T>;
using stringview_t = stl::string_view;
using string_t = stl::string;

template<class T>
using queue_t = stl::queue<T>;

template<class T>
using vector_t = stl::vector<T>;

template<class K, class T>
using umap_t = stl::unordered_map<K, T>;

template<class T>
using uset_t = stl::unordered_set<T>;

template<class K, class T>
using map_t = stl::map<K, T>;

template<class T, size_t S>
using array_t = stl::array<T, S>;

template<class T>
using deque_t = stl::deque<T>;

template<size_t TSize>
using bitset_t = stl::bitset<TSize>;

//- defining handles here as they migh have to be registered in RTTR
//------------------------------------------------------------------------------------------------------------------------
using handle_type_t		= uint64_t;
#define invalid_handle_t MAX(handle_type_t)
using service_type_t	= handle_type_t;
using entity_proxy_t	= int;
using query_t			= handle_type_t;

//------------------------------------------------------------------------------------------------------------------------
using ivec2_t = glm::lowp_u32vec2;
using ivec3_t = glm::lowp_u32vec3;
using ivec4_t = glm::lowp_u32vec4;
using vec2_t = glm::vec2;
using vec3_t = glm::vec3;
using vec4_t = glm::vec4;
using mat2_t = glm::mat2;
using mat3_t = glm::mat3x3;
using mat4_t = glm::mat4x4;
using quat_t = glm::quat;

#if defined(core_EXPORTS) && defined(CORE_USE_EASTL)
//- implementation required for EASTL. The function will be available in any application or plugin
//- linking to core, the implementation however is only exported to static library.
//- Note: using mi_aligned_alloc and mi_alloc seems to break something while using with EASTL and
//- freeing arrays does not work, using malloc is fine however.
//------------------------------------------------------------------------------------------------------------------------
void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return CORE_MALLOC(size);
}

//------------------------------------------------------------------------------------------------------------------------
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags,
	const char* file, int line)
{
	return CORE_MALLOC(size);
}
#endif

namespace rttr
{
	namespace detail
	{
		template<class T, typename = void> struct skey_type { using type = void; };
		template<class T, typename = void> struct svalue_type { using type = void; };
		template<class T> struct skey_type<T, std::void_t<typename T::key_type>> { using type = typename T::key_type; };
		template<class T> struct svalue_type<T, std::void_t<typename T::value_type>> { using type = typename T::value_type; };
		template<class T, typename = void> struct sis_iterator : std::false_type { };
		template<class T> struct sis_iterator<T, std::void_t<typename T::iterator>> : std::true_type { };
		template<class T> constexpr bool sis_container = sis_iterator<T>::value && !std::is_same_v<T, string_t>;

	} //- detail

	//- Register a default constructor for a class. Can be container types, such as vector_t<> or normal classes.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	static void default_constructor()
	{
		//- i.e. vector_t or array_t (c-style arrays are not supported)
		if constexpr (!std::is_array_v<TType>)
		{
			if (const auto type = rttr::type::get<TType>(); type.is_valid() && type.get_constructors().empty())
			{
				typename rttr::registration::template class_<TType> __class(type.get_name());
				__class.constructor()(rttr::policy::ctor::as_object);
			}
		}
		//- recursive register default constructor for i.e. map_t or nested containers, i.e. vector_t< map_t<>> etc.
		if constexpr (detail::sis_container<TType>)
		{
			using key_t = typename detail::skey_type<TType>::type;
			using value_t = typename detail::svalue_type<TType>::type;

			if constexpr (detail::sis_container<key_t>)
			{
				default_constructor<key_t>();
			}
			if constexpr (detail::sis_container<value_t>)
			{
				default_constructor<value_t>();
			}
		}
	}

} //- rttr

namespace core
{
	//- forward declarations
	class cuuid;
	struct srect;

	//- common enums etc.
	//------------------------------------------------------------------------------------------------------------------------
	enum future_status
	{
		future_status_none = 0,
		future_status_pending,	//- result is not ready yet
		future_status_ready,	//- ready, result can be retrieved
		future_status_deferred, //- result will be available when explicitly requested
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum resource_status : uint8_t
	{
		resource_status_none = 0,
		resource_status_loading,
		resource_status_ready,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum file_io_status : uint8_t
	{
		file_io_status_none = 0,
		file_io_status_pending,
		file_io_status_success,
		file_io_status_failed,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum filesystem_lookup_type : uint8_t
	{
		filesystem_lookup_type_none = 0,
		filesystem_lookup_type_directory,
		filesystem_lookup_type_file,
		filesystem_lookup_type_any,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum common_color
	{
		common_color_red_dark,
		common_color_red,
		common_color_red_light,

		common_color_orange_dark,
		common_color_orange,
		common_color_orange_light,

		common_color_yellow_dark,
		common_color_yellow,
		common_color_yellow_light,

		common_color_green_dark,
		common_color_green,
		common_color_green_light,

		common_color_cyan_dark,
		common_color_cyan,
		common_color_cyan_light,

		common_color_blue_dark,
		common_color_blue,
		common_color_blue_light,

		common_color_magenta_dark,
		common_color_magenta,
		common_color_magenta_light,

		common_color_pink_dark,
		common_color_pink,
		common_color_pink_light,

		common_color_transparent,
		common_color_neutral0,		//- black
		common_color_neutral100,
		common_color_neutral200,
		common_color_neutral300,
		common_color_neutral400,
		common_color_neutral500,
		common_color_neutral600,
		common_color_neutral800,
		common_color_neutral1000,	//- white
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum class logging_mode : uint8_t
	{
		logging_mode_console = 0,
		logging_mode_file,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum logging_verbosity : uint8_t
	{
		logging_verbosity_trace		= SPDLOG_LEVEL_TRACE,	//- internal
		logging_verbosity_debug		= SPDLOG_LEVEL_DEBUG,	//- internal
		logging_verbosity_info		= SPDLOG_LEVEL_INFO,
		logging_verbosity_warn		= SPDLOG_LEVEL_WARN,
		logging_verbosity_error		= SPDLOG_LEVEL_ERROR,
		logging_verbosity_critical	= SPDLOG_LEVEL_CRITICAL,
		logging_verbosity_off		= SPDLOG_LEVEL_OFF,
	};

	//- Structure to retrieve underlying information about platform, configuration etc.
	//------------------------------------------------------------------------------------------------------------------------
	struct sinfo
	{
		static string_t platform();
		static string_t architecture();
		static string_t compiler();
		static string_t configuration();
	};

	//- RTTR aware replacement for std::pair<>
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TKey, typename TValue>
	struct spair
	{
		spair() = default;
		~spair() = default;
		spair(TKey k, TValue v) : first(k), second(v) {}

		TKey first;
		TValue second;

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cnon_copyable
	{
	public:
		cnon_copyable() = default;
		virtual ~cnon_copyable() = default;
	private:
		cnon_copyable& operator=(const cnon_copyable&) = delete;
		cnon_copyable(const cnon_copyable&) = delete;
	};

} //- core

namespace algorithm
{
	unsigned hash(stringview_t string);
	unsigned percentage(float total_value, float part_value);
	float percent_value(unsigned p, float total_value);
	bool is_valid_handle(handle_type_t h);
	bool is_valid_proxy(entity_proxy_t h);
	handle_type_t invalid_handle();
	float bytes_to_kilobytes(unsigned b);
	float bytes_to_megabytes(unsigned b);
	float bytes_to_gigabytes(unsigned b);

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void bit_set(int& byte, T bit)
	{
		static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "Invalid operation. T must be an integral or enum type");

		byte |= static_cast<int>(bit);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void bit_clear(int& byte, T bit)
	{
		static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "Invalid operation. T must be an integral or enum type");

		byte &= ~static_cast<int>(bit);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void bit_flip(int& byte, T bit)
	{
		static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "Invalid operation. T must be an integral or enum type");

		byte ^= static_cast<int>(bit);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	bool bit_check(const int& byte, T bit)
	{
		static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "Invalid operation. T must be an integral or enum type");

		return (byte & static_cast<int>(bit)) != 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType, typename TIterator>
	bool find(TIterator begin, TIterator end, const TType& value)
	{
		return stl::find(begin, end, value) != end;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TCallable, typename TIterator>
	TIterator find_if(TIterator begin, TIterator end, TCallable&& callable)
	{
		return stl::find_if(begin, end, callable);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType, typename TIterator>
	TIterator find_at(TIterator begin, TIterator end, const TType& value)
	{
		return stl::find(begin, end, value);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType, typename TIterator>
	TType& get(TIterator begin, TIterator end, const TType& value)
	{
		auto it = stl::find(begin, end, value);

		return *it;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TEnum>
	string_t enum_to_string(TEnum value)
	{
		return magic_enum::enum_name(value).data();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TEnum>
	TEnum string_to_enum(stringview_t value)
	{
		return magic_enum::enum_cast<TEnum>(value);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TStructure>
	void swap(TStructure& swap_to, TStructure& swap_from)
	{
		stl::swap(swap_to, swap_from);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TStructure>
	void erase_at_index(TStructure& structure, unsigned index)
	{
		structure.erase(structure.begin() + index);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TStructure, typename TIterator>
	void erase_at(TStructure& structure, TIterator it)
	{
		structure.erase(it);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TStructure, typename TType>
	void erase_first(TStructure& structure, TType value)
	{
		auto it = structure.find(value);
		if (it != structure.end())
		{
			structure.erase(it);
		}
	}

	//- Iterate a data structure and call given function with the iterator as argument.
	//- If the callable returns true then we erase the element and return true, if nothing is removed we return false.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TStructure, typename TCallable>
	bool erase_if(TStructure& structure, TCallable&& function)
	{
		for (auto it = structure.begin(); it != structure.end(); ++it)
		{
			if (function(it))
			{
				structure.erase(it);
				return true;
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TStructure, typename TCallable>
	void copy_if(const TStructure& from, TStructure& to, TCallable&& function)
	{
		stl::copy_if(from.begin(), from.end(), to, function);
	}

	//- TODO: There seems to be a problem with the previous implementation. Refactor me.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TIterator>
	void shuffle(TIterator begin, TIterator end, unsigned seed = time(NULL))
	{
		std::random_device device;
		std::mt19937 generator(device());

		//static const std::default_random_engine C_RANDOM_ENGINE(seed);

		stl::shuffle(begin, end, generator);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TIterator, typename TCallable>
	void sort(TIterator begin, TIterator end, TCallable&& function)
	{
		stl::sort(begin, end, function);
	}

	namespace detail
	{
		//- @reference: https://gist.github.com/khvorov/cd626ea3685fd5e8bf14
		//------------------------------------------------------------------------------------------------------------------------
		template <typename F>
		struct function_traits : public function_traits<decltype(&F::operator())> {};

		//------------------------------------------------------------------------------------------------------------------------
		template <typename R, typename C, typename... ARGS>
		struct function_traits<R(C::*)(ARGS...) const>
		{
			using function_type = std::function<R(ARGS...)>;
		};

		//------------------------------------------------------------------------------------------------------------------------
		template<typename F>
		using function_type_t = typename detail::function_traits<F>::function_type;

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	template<typename F>
	detail::function_type_t<F> convert_to_function(F& lambda)
	{
		return static_cast<detail::function_type_t<F>>(lambda);
	}

} //- algorithm

namespace engine
{
	class cservice_manager;

} //- engine

namespace core
{
	using error_report_function_t = std::function<void(logging_verbosity, const string_t&)>;

	//------------------------------------------------------------------------------------------------------------------------
	struct serror_reporter
	{
		STATIC_INSTANCE_EX(serror_reporter);

		error_report_function_t m_callback = nullptr;
	};

} //- core

namespace rttr
{
	namespace detail
	{
		struct no_default {};

	} //- detail

	//- Utility class for RTTR registration. Adds a default constructor.
	//- Intended for classes. Use the class_() function to register metas etc.
	//- We do not check for duplicate registrations as those might be a side-effect of REFLECT_INLINE() usage.
	//-
	//- TPolicy can be one of
	//-		- rttr::detail::as_object
	//-		- rttr::detail::as_std_shared_ptr
	//-		- rttr::detail::as_raw_pointer
	//-		- rttr::detail::no_default (omits registering a default constructor, useful when you do not want one)
	//------------------------------------------------------------------------------------------------------------------------
	template<class TClass, typename TPolicy = rttr::detail::as_object>
	class cregistrator
	{
	public:
		cregistrator(rttr::string_view name);

		template<typename TMethod>
		cregistrator& meth(rttr::string_view name, TMethod method);

		template<typename TProp>
		cregistrator& prop(rttr::string_view name, TProp property);

		template<typename TCustomPolicy, typename... ARGS>
		cregistrator& ctor()
		{
			if constexpr (std::is_same_v<rttr::detail::as_object, TCustomPolicy>)
			{
				static_assert(std::is_copy_constructible_v<TClass>, "Invalid operation. Class must be copy-constructible when registered as with 'as_object' policy");

				m_object.template constructor<ARGS...>()(rttr::policy::ctor::as_object);
			}
			else if constexpr (std::is_same_v<rttr::detail::as_raw_pointer, TCustomPolicy>)
			{
				m_object.template constructor<ARGS...>()(rttr::policy::ctor::as_raw_ptr);
			}
			else if constexpr (std::is_same_v<rttr::detail::as_std_shared_ptr, TCustomPolicy>)
			{
				m_object.template constructor<ARGS...>()(rttr::policy::ctor::as_std_shared_ptr);
			}
			else
			{
				CORE_ASSERT(false, "Invalid operation. TCustomPolicy must be one of 'rttr::policy::ctor::as_object', 'rttr::policy::ctor::as_std_shared_ptr' or 'rttr::policy::ctor::as_raw_pointer'");
			}
			return *this;
		}

	private:
		rttr::registration::class_<TClass> m_object;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<class TClass, typename TPolicy /*= rttr::detail::as_object*/>
	rttr::cregistrator<TClass, TPolicy>::cregistrator(rttr::string_view name) :
		m_object(name)
	{
		if constexpr (!std::is_same_v<rttr::detail::no_default, TPolicy>)
		{
			//- class should be registered with RTTR by this point
			if constexpr (std::is_same_v<rttr::detail::as_object, TPolicy>)
			{
				static_assert(std::is_copy_constructible_v<TClass>, "Invalid operation. Class must be copy-constructible when registered as with 'as_object' policy");

				m_object.constructor()(rttr::policy::ctor::as_object);
			}
			else if constexpr (std::is_same_v<rttr::detail::as_raw_pointer, TPolicy>)
			{
				m_object.constructor()(rttr::policy::ctor::as_raw_ptr);
			}
			else if constexpr (std::is_same_v<rttr::detail::as_std_shared_ptr, TPolicy>)
			{
				m_object.constructor()(rttr::policy::ctor::as_std_shared_ptr);
			}
			else
			{
				CORE_ASSERT(false, "Invalid operation. TPolicy must be one of 'rttr::detail::as_object', 'rttr::detail::as_std_shared_ptr' or 'rttr::detail::as_raw_pointer'");
			}

			if (core::serror_reporter::instance().m_callback)
			{
				core::serror_reporter::instance().m_callback(core::logging_verbosity_error,
					fmt::format("Registering RTTR class '{}' with policy '{}'",
						rttr::type::get<TClass>().get_name().data(), rttr::type::get<TPolicy>().get_name().data()));
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TClass, typename TPolicy /*= rttr::detail::as_object*/>
	template<typename TProp>
	cregistrator<TClass, TPolicy>& rttr::cregistrator<TClass, TPolicy>::prop(rttr::string_view name, TProp property)
	{
		m_object.property(name, std::move(property));
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TClass, typename TPolicy /*= rttr::detail::as_object*/>
	template<typename TMethod>
	cregistrator<TClass, TPolicy>& rttr::cregistrator<TClass, TPolicy>::meth(rttr::string_view name, TMethod method)
	{
		m_object.method(name, std::move(method));
		return *this;
	}

} //- rttr


namespace core
{
	constexpr uint64_t C_LINKED_TREE_DEFAULT_CAPACITY = 512;

	namespace io
	{
		constexpr stringview_t C_NO_SERIALIZE_META	= "NO_SERIALIZE";
		constexpr stringview_t C_OBJECT_TYPE_NAME	= "__type__";
		constexpr stringview_t C_MAP_KEY_NAME		= "__key__";
		constexpr stringview_t C_MAP_VALUE_NAME		= "__value__";

		[[nodiscard]] rttr::variant from_json_object(rttr::type expected, const simdjson::dom::element& json);
		[[nodiscard]] rttr::variant from_json_string(rttr::type expected, const string_t& json);
		template<class TType>
		[[nodiscard]] TType from_json_string(const string_t& json)
		{
			auto var = from_json_string(rttr::type::get<TType>(), json);
			return std::move(var.template get_value<TType>());
		}
		string_t to_json_string(rttr::instance object, bool beautify = false);
		[[nodiscard]] nlohmann::json to_json_object(rttr::instance object);
		void to_json_object(rttr::instance object, nlohmann::json& json);
		[[nodiscard]] simdjson::dom::element load_json(stringview_t path);

	} //- io

	namespace string_utils
	{
		void split(const string_t& string, char delimiter, stl::vector< string_t >& storage);
		void insert(string_t& string, const string_t& to_insert_one, size_t index);
		void push_front(string_t& string, const string_t& to_prepend_one);
		void push_back(string_t& string, const string_t& to_append_one);
		void erase_substr(string_t& string, const string_t& string_to_erase);
		void erase_range(string_t& string, size_t begin, size_t end);
		void to_lower(string_t& string);
		size_t length(const string_t& string);
		size_t find_substr(const string_t& string, const string_t& substring);
		bool does_substr_exist(const string_t& string, const string_t& substring);
		bool compare(const string_t& first, const string_t& second);

		//------------------------------------------------------------------------------------------------------------------------
		inline static bool is_in_range(char c, char from, char to)
		{
			return unsigned(c - from) <= unsigned(to - from);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static bool is_space(char c)
		{
			return ' ' == c		//- Space.
				|| '\t' == c	//- Horizontal tab.
				|| '\n' == c	//- Line feed / new line.
				|| '\r' == c	//- Carriage return.
				|| '\v' == c	//- Vertical tab.
				|| '\f' == c	//- Form feed / new page.
				;
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static bool is_upper(char c)
		{
			return is_in_range(c, 'A', 'Z');
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static bool is_lower(char c)
		{
			return is_in_range(c, 'a', 'z');
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static bool is_alpha(char c)
		{
			return is_lower(c) || is_upper(c);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static bool is_numeric(char c)
		{
			return is_in_range(c, '0', '9');
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static bool is_alpha_numeric(char c)
		{
			return is_alpha(c) || is_numeric(c);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static char to_lower(char c)
		{
			return c + (is_upper(c) ? 0x20 : 0);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static char to_upper(char c)
		{
			return c - (is_lower(c) ? 0x20 : 0);
		}

	} //- string_utils

	namespace detail
	{
		//- utility to allow storing pointers to dynamic pools holding arbitrary template types
		//------------------------------------------------------------------------------------------------------------------------
		struct ipool{};

		//- Resizable and memory friendly pool providing constant access time and index stability, meaning
		//- when an element is removed other indices stay valid, so that they could be used as handles.
		//- Note: you have to deallocate memory using the shutdown function
		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		class cdynamic_pool : public ipool
		{
		public:
			cdynamic_pool();
			~cdynamic_pool() = default;

			bool init(uint64_t count);
			void shutdown();
			void reset();
			TType* create(uint64_t* index_out);
			template<typename... ARGS>
			TType* create(uint64_t* index_out, ARGS&&... args);
			TType* replace(uint64_t index);
			void destroy(uint64_t index);
			TType* begin();
			TType* advance(TType* object);
			const TType* find(uint64_t index) const;
			TType* modify(uint64_t index);
			uint64_t size() const;
			uint64_t capacity() const;
			bool empty() const;
			uint64_t memory_usage() const;
			uint64_t memory_reserved() const;

		private:
			vector_t<bool> m_initialized_bit;
			std::stack<unsigned> m_fragmentation_indices;
			uint64_t m_top;
			uint64_t m_size;
			uint64_t m_capacity;
			void* m_start;
			void* m_end;

		private:
			void resize(uint64_t count);
			TType* unsafe(uint64_t index);
			const TType* unsafe(uint64_t index) const;
			bool initialized_at_index(uint64_t index) const;
		};

	} //- detail

	//- base class for a service
	//------------------------------------------------------------------------------------------------------------------------
	class cservice
	{
		friend class engine::cservice_manager;
	public:
		static constexpr service_type_t C_INVALID_TYPE = invalid_handle_t;
		static constexpr unsigned C_SERVICE_COUNT_MAX = 64;

		cservice() = default;
		virtual ~cservice() = default;

		virtual bool on_start() { return false; }
		virtual void on_shutdown() {}
		virtual void on_update(float) {}

		RTTR_ENABLE();
	};

	//- Class wrapping a future type. If copied from another future then both are waiting for the same result.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	class cfuture_type
	{
	public:
		cfuture_type(const std::future<TType>& task) : m_task(task.share()), m_status(future_status_pending) {}
		cfuture_type(const std::shared_future<TType>& task) : m_task(task), m_status(future_status_pending) {}
		cfuture_type(const cfuture_type<TType>& other) : m_task(other.m_task), m_status(other.m_status) {}
		cfuture_type() = default;
		~cfuture_type() = default;

		bool ready();
		[[nodiscard]] TType get() const { ASSERT(ready(), "Invalid operation. You can use 'get' only if future is ready"); return m_task.get(); }

	private:
		std::shared_future<TType> m_task;
		future_status m_status = future_status_none;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	bool core::cfuture_type<TType>::ready()
	{
		using namespace std::chrono_literals;

		switch (m_task.wait_for(1us))
		{
		case std::future_status::ready:
		{
			m_status = future_status_ready;
			break;
		}
		case std::future_status::deferred:
		{
			m_status = future_status_deferred;
			break;
		}
		default:
		case std::future_status::timeout:
		{
			m_status = future_status_pending;
			break;
		}
		}
		return m_status == future_status_ready;
	}

	//- Usable for async operations where using thread service is not possible
	//------------------------------------------------------------------------------------------------------------------------
	class casync final
	{
	public:
		template<typename TCallable>
		static void launch_silent(TCallable&& function) { launch(function); }

		template<typename TCallable>
		static decltype(auto) launch_async(TCallable&& function) { return launch(function); }

	private:
		template<typename TCallable>
		static [[maybe_unused]] decltype(auto) launch(TCallable&& function)
		{
			static tf::Executor C_EXECUTOR;

			return C_EXECUTOR.async(function);
		}
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cuuid final
	{
	public:
		static const cuuid C_INVALID_UUID;
		static const array_t<unsigned char, 16u> C_INVALID_DATA;

		cuuid();
		cuuid(array_t<unsigned char, 16u> data);
		cuuid(const string_t& uuid);
		cuuid(size_t seed);
		cuuid(const cuuid& other);
		~cuuid() = default;

		string_t string() const { return generate_string(); }
		unsigned hash() const { return algorithm::hash(string().c_str()); }
		bool is_equal_to(const cuuid& uuid) const { return compare(uuid) == 0; }
		bool is_smaller_as(const cuuid& uuid) const { return compare(uuid) < 0; }
		bool is_higher_as(const cuuid& uuid) const { return compare(uuid) > 0; }
		bool operator==(const cuuid& other) const { return is_equal_to(other); }
		bool operator!=(const cuuid& other) const { return !is_equal_to(other); };

	private:
		inline static const auto C_RANDOM_BYTES_COUNT = 4;
		inline static const unsigned char C_HEX[] = "0123456789abcdef";

		array_t<unsigned char, 16u> m_data = C_INVALID_DATA;

	private:
		void generate(size_t seed);
		void parse_string(const string_t& uuid, array_t<unsigned char, 16u>& out);
		void write_string(const array_t<unsigned char, 16>& data, string_t& out) const;
		unsigned hex2dec(char c);
		void copy_to(cuuid& other);
		void copy_from(const cuuid& other);
		int compare(const cuuid& other) const;
		string_t generate_string() const;

		RTTR_ENABLE();
		RTTR_REFLECTABLE();
	};

	//- random number generator
	//------------------------------------------------------------------------------------------------------------------------
	class crandom final
	{
	public:
		crandom() = default;
		~crandom() = default;

		static void seed(unsigned value = 0);

		//- retrieve a random number deviating in range of given value by plus a positive amount or
		//- by minus a negative amount
		int random_deviation(int value, int negative, int positive);

		float random_float();
		unsigned random_int();
		float alternate_one_float();
		int alternate_one_int();
		float normalized_float();
		float in_range_float(float minimum, float maximum);
		unsigned in_range_int(unsigned minimum, unsigned maximum);
		vec2_t normalized_vec2();
		vec2_t in_range_vec2(float minimum, float maximum);

	private:
		inline static std::mt19937 C_RANDOM_ENGINE;
		std::uniform_int_distribution<std::mt19937::result_type> m_distribution;
	};

	//- crossplatform and minimal mutex class: basically from bx
	//------------------------------------------------------------------------------------------------------------------------
	class cmutex final
	{
	public:
		cmutex();
		~cmutex();

		void lock();
		void unlock();

	private:
#if CORE_PLATFORM_WINDOWS
		__declspec(align(16)) uint8_t m_internal[64];
#else
		__attribute__((aligned (16))) uint8_t m_internal[64];
#endif
	};

	//------------------------------------------------------------------------------------------------------------------------
	class  cscope_mutex final
	{
	public:
		cscope_mutex(cmutex& m);
		~cscope_mutex();
		cscope_mutex& operator=(const cscope_mutex&) = delete;
		cscope_mutex(const cscope_mutex&) = delete;

	private:
		cmutex& m_mutex;
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct scolor final
	{
		scolor(common_color color);
		scolor(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 0);
		scolor(vec4_t normalized);

		inline uint8_t r() const { return m_r; }
		inline uint8_t g() const { return m_g; }
		inline uint8_t b() const { return m_b; }
		inline uint8_t a() const { return m_a; }

		template<typename T>
		inline T cliteral() const { return { m_r, m_g, m_b, m_a }; }

		vec4_t normalize() const;

		unsigned rgba() const;
		unsigned abgr() const;

		uint8_t m_r, m_g, m_b, m_a;

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct srect final
	{
		srect(const vec2_t& xy, const vec2_t& wh);
		srect(float x = 0.0f, float y = 0.0f, float w = 0.0f, float h = 0.0f);

		float x() const { return m_x; };
		float y() const { return m_y; };
		float w() const { return m_w; };
		float h() const { return m_h; };

		vec2_t top_left() const { return { m_x, m_y }; }
		vec2_t top_right() const { return { m_x + m_w, m_y }; }
		vec2_t bottom_left() const { return { m_x, m_y + m_h }; }
		vec2_t bottom_right() const { return { m_x + m_w, m_y + m_h }; }

		void set(float x, float y, float w, float h);
		void position(float x, float y);
		void dimension(float w, float h);

		float m_x, m_y, m_w, m_h;

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cpath final
	{
	public:
		cpath(const char* path);
		cpath(const std::filesystem::path& path);
		cpath(const cpath& path);
		cpath& operator=(const cpath& path);
		~cpath() = default;

		std::filesystem::path path() const;
		std::filesystem::directory_entry dir() const;

		inline const char* view() const { return m_string_path.c_str(); }
		inline const char* extension() const { return m_string_ext.c_str(); }
		inline const char* stem() const { return m_string_stem.c_str(); }

		bool exists() const;
		explicit operator bool() const { return exists(); }

		bool has_extension() const;
		bool has_parent() const;
		bool is_dir() const;
		bool is_file() const;

		cpath parent() const;
		cpath& append(stringview_t path);
		cpath& operator /=(stringview_t path);
		bool operator==(stringview_t path);
		bool operator==(const std::filesystem::path& path);

	private:
		string_t m_string_path;
		string_t m_string_ext;
		string_t m_string_stem;
		std::filesystem::path m_path;
		std::filesystem::directory_entry m_dir;

	private:
		//- TODO: Reconsider later how to deal with filesystem wchar_t and ditch the 3 strings
		void update_strings();
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cfilesystem final
	{
	public:
		cfilesystem(cpath path);
		cfilesystem(const char* path);
		~cfilesystem() = default;

		static cpath construct(const char* path, const char* addition);
		static cpath join(const char* path, const char* addition);
		static cpath cwd();

		static bool create_dir(const char* path);
		static bool create_dir_in(const char* path, const char* name);
		static bool create_dir_recursive(const char* path);

		static bool create_file(const char* path);
		static bool create_file_in(const char* path, const char* stem, const char* ext);

		static bool remove(const char* path);
		static bool remove(const char* path, const char* name);

		static bool find_file(const char* path, const char* name);
		static bool find_file_by_stem(const char* path, const char* name);
		static bool find_file_by_extension(const char* path, const char* name);
		static bool find_dir(const char* path, const char* name);
		static bool find_at(const char* path, const char* name, filesystem_lookup_type type);

		static cpath construct_relative_to_cwd(const char* path);
		static bool is_contained(const cpath& contained, const cpath& container);

		inline cpath current() const { return m_current; }

		bool forwards(const char* path, bool forced = false);
		bool backwards();

		void append(const char* path);
		cfilesystem& operator/=(const char* path);

		inline bool exists() const { return m_current.exists(); };
		inline bool is_file() const { return m_current.is_file(); };
		inline bool is_dir() const { return m_current.is_dir(); };

	private:
		cpath m_current;
	};

	//- Helper class for performing I/O operations both sync and async.
	//- Note: when you are loading something, you are responsible for unloading it afterwards.
	//------------------------------------------------------------------------------------------------------------------------
	class cfile final
	{
	public:
		static string_t load_text(const string_t& path);

		static std::future<string_t> load_text_async(const string_t& path);

		static file_io_status save_text(const string_t& path, const string_t& text);

		static std::future<file_io_status> save_text_async(const string_t& path, const string_t& text);

		static spair<uint8_t*, unsigned> load_binary(const string_t& path);

		static uint8_t* load_binary(const string_t& path, unsigned& size_out);

		static std::future<spair<uint8_t*, unsigned>> load_binary_async(const string_t& path);

		static file_io_status save_binary(const string_t& path, uint8_t* data, unsigned size);

		static std::future<file_io_status> save_binary_async(const string_t& path, uint8_t* data, unsigned size);

		static void unload(char* data);

		static void unload(void* data);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class ctimer final
	{
	public:
		ctimer(bool paused = true);
		~ctimer() = default;

		void start();
		bool started() const;
		float secs() const;
		float millisecs() const;
		float microsecs() const;

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_timepoint;
	};

	//- Note: data type the class is holding must be copy-constructible
	//------------------------------------------------------------------------------------------------------------------------
	class cany final
	{
	public:
		cany() = default;
		cany(rttr::variant var) : m_data(std::move(var)) { }
		cany& operator=(const cany& other);
		~cany();

		inline rttr::type::type_id type() const { return m_data.get_type().get_id(); }
		inline stringview_t type_name() const { return m_data.get_type().get_name().data(); }
		inline bool empty() const { return !m_data.is_valid(); }

		template<typename TType>
		inline bool is() const
		{
			return rttr::type::get<TType>().get_id() == type();
		}

		template<typename TType>
		TType copy()
		{
			ASSERT(is<TType>(), "Casting to another data type is not allowed");

			return m_data.get_value<TType>();
		}

		template<typename TType>
		TType& get()
		{
			ASSERT(is<TType>(), "Casting to another data type is not allowed");

			return m_data.get_value<TType>();
		}

		template<typename TType>
		const TType& get() const
		{
			ASSERT(is<TType>(), "Casting to another data type is not allowed");

			return m_data.get_value<TType>();
		}

	private:
		rttr::variant m_data;
	};

	//- TODO: still work-in-progress
	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	class clinked_tree
	{
	public:
		struct snode : public TType
		{
			snode* m_parent = nullptr;
			vector_t<snode*> m_children;
		};

		using visitor_func_t = std::function<bool(snode*)>;

		clinked_tree(uint64_t size = C_LINKED_TREE_DEFAULT_CAPACITY)
		{
			m_pool.init(size);

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_debug,
					fmt::format("Creating clinked_tree with reserved capacity of '{}({}KB)'",
						m_pool.capacity(), algorithm::bytes_to_kilobytes(m_pool.memory_reserved())));
			}
		}

		~clinked_tree()
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_debug,
					fmt::format("Destroying clinked_tree of size '{}({}KB)' and capacity '{}({}KB)'",
						m_pool.size(), algorithm::bytes_to_kilobytes(m_pool.memory_usage()),
						m_pool.capacity(), algorithm::bytes_to_kilobytes(m_pool.memory_reserved())));
			}

			m_pool.shutdown();
		}

		void visit_depth_first(visitor_func_t visitor)
		{
			if (!m_pool.empty())
			{
				std::stack<snode*> stack;
				stack.push(m_pool.begin());

				while (!stack.empty())
				{
					auto n = stack.top();
					stack.pop();

					if (!visitor(n)) { break; }

					for (const auto& k : n->m_children)
					{
						stack.push(k);
					}
				}
			}
		}

		template<typename TCallable>
		void visit_depth_first(TCallable&& lambda)
		{
			visit_depth_first(algorithm::convert_to_function(lambda));
		}

		snode* append_to(snode* node = nullptr, uint64_t* index_out = nullptr)
		{
			node = node ? node : &m_root;
			
			auto* n = m_pool.create(index_out);

			n->m_parent = node;
			node->m_children.push_back(n);

			return n;
		}

		const snode* find(uint64_t index) const
		{
			return m_pool.find(index);
		}

		snode* modify(uint64_t index)
		{
			return m_pool.modify(index);
		}

		snode* root() const
		{
			return m_pool.begin();
		}

		bool empty() const {return m_pool.empty();}
		uint64_t size() const { return m_pool.size(); }
		uint64_t capacity() const { return m_pool.capacity(); }

	private:
		detail::cdynamic_pool<snode> m_pool;
		snode m_root;
	};

} //- core

namespace core
{
	namespace detail
	{
		namespace
		{
			//- Note: returns true for 0 not to trigger anything
			//------------------------------------------------------------------------------------------------------------------------
			inline static bool is_power_of_2(uint64_t n)
			{
				return n > 0 ? (n & (n - 1)) == 0 : true;
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static uint64_t next_power_of_2(uint64_t n)
			{
				n--;
				n |= n >> 1;
				n |= n >> 2;
				n |= n >> 4;
				n |= n >> 8;
				n |= n >> 16;
				n++;
				return n;
			}

			//- Calculate amount of bytes required for the given address to be aligned.
			//------------------------------------------------------------------------------------------------------------------------
			inline static uint64_t align_address_forward(uint64_t address, uint64_t alignment)
			{
				CORE_ASSERT(alignment > 0, "Invalid operation. Alignment must be greater than 0!");

				auto result = address;
				const auto modulo = address & (address - 1);

				if (modulo != 0)
				{
					result += alignment - modulo;
				}

				return result;
			}

			//- Calculate amount of bytes required for the given address to be aligned including the size of the header.
			//- Note: alignment must be greater than 0.
			//------------------------------------------------------------------------------------------------------------------------
			template<typename THeader>
			inline static uint64_t align_address_forward_with_header(uint64_t address, uint64_t alignment)
			{
				CORE_ASSERT(alignment > 0, "Invalid operation. Alignment must be greater than 0!");

				auto padding = align_address_forward(address, alignment);
				auto required_space = sizeof(THeader);

				if (padding < required_space)
				{
					//- the header does not fit, so we calculate next aligned address in which it fits
					required_space -= padding;

					if (required_space % alignment > 0)
					{
						padding += alignment * (1u + (required_space / alignment));
					}
					else
					{
						padding += alignment * (required_space / alignment);
					}
				}

				return padding;
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static uint64_t memloc(void* m)
			{
				return reinterpret_cast<uint64_t>(m);
			}

			//------------------------------------------------------------------------------------------------------------------------
			inline static uint64_t memloc_index(void* m, void* start, uint64_t object_size)
			{
				return (reinterpret_cast<uint64_t>(m) - reinterpret_cast<uint64_t>(start)) / object_size;
			}



		}//- unnamed

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		core::detail::cdynamic_pool<TType>::cdynamic_pool() :
			m_top(0), m_size(0), m_capacity(0), m_start(nullptr), m_end(nullptr)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		TType* core::detail::cdynamic_pool<TType>::unsafe(uint64_t index)
		{
			return reinterpret_cast<TType*>(reinterpret_cast<void*>((reinterpret_cast<uint64_t>(m_start) + index * sizeof(TType))));
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		const TType* core::detail::cdynamic_pool<TType>::unsafe(uint64_t index) const
		{
			return reinterpret_cast<TType*>(reinterpret_cast<void*>((reinterpret_cast<uint64_t>(m_start) + index * sizeof(TType))));
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		void core::detail::cdynamic_pool<TType>::resize(uint64_t count)
		{
			auto prev_start = m_start;
			auto prev_count = m_size;

			m_start = CORE_MALLOC(count * sizeof(TType));

			m_end = reinterpret_cast<void*>(memloc(m_start) + count * SCAST(uint64_t, sizeof(TType)));

			m_capacity = count;

			m_initialized_bit.resize(count, false);

			std::memmove(m_start, prev_start, prev_count * sizeof(TType));

			CORE_FREEN(prev_start, prev_count);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		uint64_t core::detail::cdynamic_pool<TType>::capacity() const
		{
			return m_capacity;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		uint64_t core::detail::cdynamic_pool<TType>::memory_usage() const
		{
			return size() * sizeof(TType);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		uint64_t core::detail::cdynamic_pool<TType>::memory_reserved() const
		{
			return capacity() * sizeof(TType);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		bool core::detail::cdynamic_pool<TType>::empty() const
		{
			return m_size == 0;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		uint64_t core::detail::cdynamic_pool<TType>::size() const
		{
			return m_size;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		TType* core::detail::cdynamic_pool<TType>::modify(uint64_t index)
		{
			CORE_NAMED_ZONE("cdynamic_pool::modify");

			return unsafe(index);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		const TType* core::detail::cdynamic_pool<TType>::find(uint64_t index) const
		{
			CORE_NAMED_ZONE("cdynamic_pool::find");

			return unsafe(index);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		bool core::detail::cdynamic_pool<TType>::initialized_at_index(uint64_t index) const
		{
			return  index < capacity() && m_initialized_bit[index];
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		TType* core::detail::cdynamic_pool<TType>::advance(TType* object)
		{
			CORE_NAMED_ZONE("cdynamic_pool::advance");

			auto next = memloc(object) + sizeof(TType);

			while (next <= RCAST(uint64_t, m_end) && !initialized_at_index(memloc_index(RCAST(void*, next), m_start, sizeof(TType))))
			{
				next += sizeof(TType);
			}

			return next <= reinterpret_cast<uint64_t>(m_end) ? reinterpret_cast<TType*>(next) : nullptr;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		TType* core::detail::cdynamic_pool<TType>::begin()
		{
			CORE_NAMED_ZONE("cdynamic_pool::begin");

			if (m_size == 0)
			{
				return nullptr;
			}

			if (!m_initialized_bit[0])
			{
				return advance(reinterpret_cast<TType*>(m_start));
			}

			return reinterpret_cast<TType*>(m_start);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		void core::detail::cdynamic_pool<TType>::destroy(uint64_t index)
		{
			if (m_size > 0)
			{
				const auto fragmentation = index < m_size - 1;

				TType* object = unsafe(index);

				object->~TType();

				m_initialized_bit[index] = false;

				--m_size;

				if (fragmentation)
				{
					m_fragmentation_indices.push(index);
				}
				else
				{
					--m_top;
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		TType* core::detail::cdynamic_pool<TType>::replace(uint64_t index)
		{
			TType* object = unsafe(index);

			object->~TType();

			object = new (object) TType;

			return object;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		template<typename... ARGS>
		TType* core::detail::cdynamic_pool<TType>::create(uint64_t* index_out, ARGS&&... args)
		{
			CORE_NAMED_ZONE("cdynamic_pool::create");

			if (size() == capacity())
			{
				resize(m_size * 2);
			}

			uint64_t index = 0;

			if (!m_fragmentation_indices.empty())
			{
				index = m_fragmentation_indices.top();

				m_fragmentation_indices.pop();
			}
			else
			{
				index = m_top++;
			}

			TType* object = unsafe(index);

			object = new (object) TType(std::forward<ARGS>(args)...);

			m_initialized_bit[index] = true;
			++m_size;

			if (index_out)
			{
				*index_out = index;
			}

			return object;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		TType* core::detail::cdynamic_pool<TType>::create(uint64_t* index_out)
		{
			CORE_NAMED_ZONE("cdynamic_pool::create");

			if (size() == capacity())
			{
				resize(m_size * 2);
			}

			uint64_t index = 0;

			if (!m_fragmentation_indices.empty())
			{
				index = m_fragmentation_indices.top();

				m_fragmentation_indices.pop();
			}
			else
			{
				index = m_top++;
			}

			TType* object = unsafe(index);

			object = new (object) TType;

			m_initialized_bit[index] = true;
			++m_size;

			if (index_out)
			{
				*index_out = index;
			}

			return object;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		void core::detail::cdynamic_pool<TType>::reset()
		{
			auto i = 0;
			auto object = begin();
			while (i < m_size)
			{
				object->~TType();

				object = advance(object);

				++i;
			}

			std::memset(m_start, NULL, m_size);

			std::fill(m_initialized_bit.begin(), m_initialized_bit.end(), false);

			m_fragmentation_indices = std::stack<unsigned>();

			m_size = 0;
			m_capacity = 0;
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		void core::detail::cdynamic_pool<TType>::shutdown()
		{
			auto* object = begin();
			while (object)
			{
				object->~TType();

				object = advance(object);
			}

			CORE_FREEN(m_start, m_size);
		}

		//------------------------------------------------------------------------------------------------------------------------
		template<class TType>
		bool core::detail::cdynamic_pool<TType>::init(uint64_t count)
		{
			m_start = CORE_MALLOC(sizeof(TType) * count);

			m_end = reinterpret_cast<void*>(memloc(m_start) + count * SCAST(uint64_t, sizeof(TType)));

			m_capacity = count;

			m_initialized_bit.resize(count, false);

			return true;
		}

	} //- detail

	//- Service manager to be used in-engine. Can hold an arbitrary amount of service types.
	//------------------------------------------------------------------------------------------------------------------------
	class cservice_manager
	{
	public:
		struct sconfig
		{
			vector_t<string_t> m_services;

			RTTR_ENABLE();
		};

		static void init();
		static void shutdown();
		static void on_update(float dt);

		template<class TService>
		static TService* find(stringview_t service_type);

		template<class TService>
		static TService* find();

		template<class TService>
		static TService* emplace();

		static bool emplace(rttr::type service_type);

	private:
		inline static int s_service_count = 0;
		inline static service_type_t s_next_type = 0;
		inline static umap_t<size_t, service_type_t> s_service_types;
		inline static array_t<rttr::variant, core::cservice::C_SERVICE_COUNT_MAX> s_services;

	private:
		static core::cservice* get_base_service(rttr::variant& var);
	};

	//- service_type name is required for correct casting of derived classes, i.e.
	//- find<sm::icamera_manager>("ccamera_manager"), where sm::icamera_manager is to what we cast and
	//- "ccamera_manager" is the underlying stored type.
	//- Useful for inheritance indirection, i.e.
	//-
	//- cservice <- derive -- icamera_manager <- derive -- ccamera_manager
	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	TService* cservice_manager::find(stringview_t service_type)
	{
		auto id = rttr::type::get_by_name(service_type.data()).get_id();

		if (s_service_types.find(id) != s_service_types.end())
		{
			auto& var = s_services[s_service_types[id]];

			ASSERT(var.is_valid(), "Invalid operation. Trying to retrieve a released service");

			return reinterpret_cast<TService*>(get_base_service(var));
		}

		ASSERT(false, "Invalid operation. Service does not exist");

		return nullptr;
	}

	//- cast to given service type without inheritance indirection.
	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	TService* cservice_manager::find()
	{
		auto id = rttr::type::get<TService>().get_id();

		if (s_service_types.find(id) != s_service_types.end())
		{
			auto& var = s_services[s_service_types[id]];

			ASSERT(var.is_valid(), "Invalid operation. Trying to retrieve a released service");

			return reinterpret_cast<TService*>(get_base_service(var));
		}

		ASSERT(false, "Invalid operation. Service does not exist");

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	TService* cservice_manager::emplace()
	{
		if (s_next_type < core::cservice::C_SERVICE_COUNT_MAX)
		{
			auto type = rttr::type::get<TService>();
			auto id = type.get_id();

			auto t = s_next_type++;
			s_services[t] = type.create({});
			s_service_types[id] = t;
			s_service_count++;
			return reinterpret_cast<TService*>(get_base_service(s_services[t]));
		}

		ASSERT(false, "Invalid operation. Maximum Service count reached, increase the count in 'core' library");

		return nullptr;
	}

	//- Class for user defined events. Does not handle file change detection (see filewatcher), nor hardware events
	//- i.e. keyboard input, mouse input, window resizing and the like (see sdl service for that).
	//-
	//- struct ExampleEvent {
	//-		float x = 0.0f, y = 0.0f;
	//-		int time = 0;
	//- };
	//-
	//- emitting an event is easy as
	//-		event_service.emit_event<ExampleEvent>();
	//-		event_service.emit_event<smy_event>(1.0f, 25.0f, 11101929);
	//-
	//- creating a listener can be done with
	//-		event_service.emplace_listener<ExampleEvent>([](const rttr::variant& var)
	//-			{
	//-				const auto& e = var.convert<ExampleEvent>();
	//-
	//-				logging::log_warn(fmt::format("Event: '{} - x={}, y={}, time={}'",
	//-					var.get_type().get_name().data(), e.x, e.y, e.time));
	//-			});
	//-
	//- Note: when handling an event and you want to hang on to that data, you can copy the event object.
	//------------------------------------------------------------------------------------------------------------------------
	class cevent_service final : public core::cservice
	{
	public:
		using event_listener_callback_t = std::function<void(const rttr::variant&)>;

		cevent_service() = default;
		~cevent_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		template<typename TEvent>
		void emit_event();

		template<typename TEvent, typename... ARGS>
		void emit_event(ARGS&&... args);

		template<typename TEvent>
		void emplace_listener(event_listener_callback_t callback);

	private:
		umap_t<rttr::type, vector_t<event_listener_callback_t>> m_listeners;
		umap_t<rttr::type, queue_t<rttr::variant>> m_queue;
		core::cmutex m_mutex;

		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TEvent>
	void cevent_service::emit_event()
	{
		core::cscope_mutex m(m_mutex);

		m_queue[rttr::type::get<TEvent>()].push(std::move(TEvent{}));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TEvent, typename... ARGS>
	void cevent_service::emit_event(ARGS&&... args)
	{
		core::cscope_mutex m(m_mutex);

		m_queue[rttr::type::get<TEvent>()].push(std::move(TEvent{ args... }));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TEvent>
	void cevent_service::emplace_listener(event_listener_callback_t callback)
	{
		core::cscope_mutex m(m_mutex);

		m_listeners[rttr::type::get<TEvent>()].emplace_back(std::move(callback));
	}

	//- Interface class for an allocator implementation.
	//------------------------------------------------------------------------------------------------------------------------
	class CORE_NO_VTABLE iallocator
	{
	public:
		static constexpr uint64_t C_ALIGNMENT = 8;

		//- in-place construct an object. Should be used for allocated objects that need additional construction.
		template<typename TObjectType, typename... ARGS>
		static TObjectType* construct(TObjectType* pointer, ARGS&&... args)
		{
			return new(SCAST(void*, pointer)) TObjectType(std::forward<ARGS>(args)...);
		}

		//- destruct object that requires destruction.
		template<typename TObjectType>
		static void destroy(TObjectType* pointer)
		{
			CORE_ASSERT(pointer, "Invalid operation. Pointer must be a valid object!");
			pointer->~TObjectType();
		}

		virtual ~iallocator() = 0;
		virtual void* allocate(uint64_t size, uint64_t alignment = iallocator::C_ALIGNMENT) = 0;
		virtual void deallocate(void* ptr) = 0;

		uint64_t capacity() const;
		uint64_t used() const;
		uint64_t peak_used() const;

	protected:
		uint64_t m_total_size = 0;		//- amount of available memory
		uint64_t m_used_size = 0;		//- current bytes in use
		uint64_t m_peak_used_size = 0;	//- biggeest amount of memory used

	protected:
		void init(uint64_t size);
		void track_allocation(int64_t allocation_size, uint64_t padding = 0);
	};

	//- Allocator also known as an 'Arena'. Performs allocations in a linear manner in a contiguous region of memory.
	//- Note that it does not deallocate anything, it will free all used memory once out of scope.
	//------------------------------------------------------------------------------------------------------------------------
	class clinear_allocator final : public iallocator
	{
	public:
		clinear_allocator(uint64_t size);
		~clinear_allocator();

		void* allocate(uint64_t size, uint64_t alignment = iallocator::C_ALIGNMENT) override final;
		void deallocate(void* ptr) override final;

	private:
		uint64_t m_offset;
		void* m_memory;
	};

	//- Allocator allowing for push and pop operations. Performs allocations in a linear manner in a contiguous region of memory.
	//------------------------------------------------------------------------------------------------------------------------
	class cstack_allocator final : public iallocator
	{
	public:
		cstack_allocator(uint64_t size);
		~cstack_allocator();

		void* allocate(uint64_t size, uint64_t alignment = iallocator::C_ALIGNMENT) override final;
		void deallocate(void* ptr) override final;

	private:
		struct sheader
		{
			uint8_t m_padding = 0;
			uint64_t m_size = 0;
		};

		uint64_t m_offset;
		void* m_memory;
	};

} //- core

namespace events
{
	namespace window
	{
		struct sresize { int w = 0; int h = 0;};
		struct sminimize {};
		struct sunminimize {};
		struct shide {};
		struct sunhide {};
		struct sfocus {};
		struct sunfocus {};

	} //- window

} //- events

namespace math
{
	constexpr mat4_t C_MAT4_ID				= mat4_t(1.0f);
	constexpr vec2_t C_VEC_UP				= vec2_t(0.0f, -1.0f);
	constexpr vec2_t C_VEC_RIGHT			= vec2_t(1.0f, 0.0f);
	constexpr float C_ALMOST_EQUAL_EPSILON	= 0.04f;
	constexpr float C_EPSILON				= 0.0000001f;

	bool almost_equal(float a, float b, float e = C_ALMOST_EQUAL_EPSILON);
	bool almost_equal(const vec2_t& a, const vec2_t& b, float e = C_ALMOST_EQUAL_EPSILON);

	//- Transforms given position, scale, shear and rotation and returns world space transform matrix
	//- TODO: shear
	//------------------------------------------------------------------------------------------------------------------------
	void transform(mat4_t& out, const vec2_t& position, const vec2_t& scale, const vec2_t& shear,
		float rotation, const mat4_t& parent = C_MAT4_ID);

	//- Transforms given position, scale, shear and rotation and returns final world space position, scale and rotation
	//- ready to be used for rendering
	//------------------------------------------------------------------------------------------------------------------------
	std::tuple<vec2_t, vec2_t, float> transform(const vec2_t& position, const vec2_t& scale, const vec2_t& shear,
		float rotation, const mat4_t& parent = C_MAT4_ID);

} //- math


namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cevent_service)
	{
		rttr::registration::class_<cevent_service>("cevent_service")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cservice_manager::sconfig)
	{
		rttr::registration::class_<cservice_manager::sconfig>("cservice_manager::sconfig")
			.property("m_services", &cservice_manager::sconfig::m_services)
			;

		rttr::default_constructor<cservice_manager::sconfig>();
		rttr::default_constructor<vector_t<string_t>>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cservice)
	{
		rttr::registration::class_<cservice>("cservice")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			.method("on_start", &cservice::on_start)
			.method("on_shutdown", &cservice::on_shutdown)
			.method("on_update", &cservice::on_update)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cuuid)
	{
		rttr::registration::class_<cuuid>("cuuid")
			.constructor<>()
			(
				rttr::policy::ctor::as_object
			)
			.property("m_data", &cuuid::m_data)
			;

		rttr::default_constructor<array_t<unsigned char, 16u>>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(scolor)
	{
		rttr::registration::class_<scolor>("scolor")
			.constructor<>()
			(
				rttr::policy::ctor::as_object
			)
			.property("m_r", &scolor::m_r)
			.property("m_g", &scolor::m_g)
			.property("m_b", &scolor::m_b)
			.property("m_a", &scolor::m_a)
			;

	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(srect)
	{
		rttr::registration::class_<srect>("srect")
			.constructor<>()
			(
				rttr::policy::ctor::as_object
			)
			.property("m_x", &srect::m_x)
			.property("m_y", &srect::m_y)
			.property("m_w", &srect::m_w)
			.property("m_h", &srect::m_h)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(vec2_t)
	{
		rttr::registration::class_<vec2_t>("vec2_t")
			.constructor<>()
			(
				rttr::policy::ctor::as_object
			)
			.property("x", &vec2_t::x)
			.property("y", &vec2_t::y)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(vec3_t)
	{
		rttr::registration::class_<vec3_t>("vec3_t")
			.constructor<>()
			(
				rttr::policy::ctor::as_object
			)
			.property("x", &vec3_t::x)
			.property("y", &vec3_t::y)
			.property("z", &vec3_t::z)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(vec4_t)
	{
		rttr::registration::class_<vec4_t>("vec4_t")
			.constructor<>()
			(
				rttr::policy::ctor::as_object
			)
			.property("x", &vec4_t::x)
			.property("y", &vec4_t::y)
			.property("z", &vec4_t::z)
			.property("w", &vec4_t::w)
			;
	}

} //- core
