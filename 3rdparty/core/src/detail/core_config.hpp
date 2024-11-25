#pragma once
#include "core_platform.hpp"
#include <vector>
#include <array>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <deque>
#include <bitset>
#include <list>
#include <stack>
namespace stl = std;
#include <glm.h>
#include <magic_enum.h>
#include <taskflow.h>
#include <spdlog.h>
#include <rttr.h>
#include <nlohmann.h>
#include <../src/simdjson.h>
#include <../src/tracy.hpp>
#define ASIO_NO_EXCEPTIONS
#include <asio.h>
namespace miniz
{
#include <../src/miniz.hpp>
} //- miniz
#include <mimalloc.h>
#include <mimalloc/types.h>
#include <hwinfo.h>		//- Used only for CPU information at this time
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
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define CORE_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define CORE_FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#define CORE_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define CORE_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define CORE_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define CORE_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define CORE_FUNC_SIG __func__
#else
#define CORE_FUNC_SIG "CORE_FUNC_SIG unknown"
#endif

//------------------------------------------------------------------------------------------------------------------------
#define STRINGIFY(s) #s
#define STRING(s) STRINGIFY(s)
#define SSTRING(s) STRING(s)

//------------------------------------------------------------------------------------------------------------------------
#define __CONCAT_IMPL__(a, b) a##b
#define CONCAT(a, b) __CONCAT_IMPL__(a, b)
#ifdef __COUNTER__
#define ANONYMOUS_VARIABLE(str) CONCAT(str, __COUNTER__)
#else
#define ANONYMOUS_VARIABLE(str) CONCAT(str, __LINE__)
#endif

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
#define ARRAYSIZE(__array) ((unsigned)(sizeof(__array) / sizeof(*(__array))))
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
#elif MIMALLOC_ENABLE
	//------------------------------------------------------------------------------------------------------------------------
#define CORE_MALLOC(size)		mi_malloc(size)
#define CORE_CALLOC(n, size)	mi_calloc(n, size)
#define CORE_REALLOC(p, size)	mi_realloc(p, size)
#define CORE_MALLOCA(size, a)	mi_aligned_alloc(a, size)
#define CORE_FREE(p)			mi_free(p)
#define CORE_FREEN(p, n)		CORE_FREE(p)
#else
	//------------------------------------------------------------------------------------------------------------------------
#define CORE_MALLOC(size)		std::malloc(size)
#define CORE_CALLOC(n, size)	std::calloc(n, size)
#define CORE_REALLOC(p, size)	std::realloc(p, size)
#define CORE_MALLOCA(size, a)	std::aligned_alloc(a, size)
#define CORE_FREE(p)			std::free(p)
#define CORE_FREEN(p, n)		CORE_FREE(p)
#endif

//------------------------------------------------------------------------------------------------------------------------
#if defined(core_EXPORTS)
#if CORE_PLATFORM_WINDOWS && TRACY_ENABLE
void* operator new(unsigned long long n);
void operator delete(void* p);
#endif
#endif

//------------------------------------------------------------------------------------------------------------------------
#if PROFILE_ENABLE
	#if CORE_PLATFORM_WINDOWS && TRACY_ENABLE
		#define CORE_ZONE ZoneScoped
		#define CORE_NAMED_ZONE(name) ZoneScopedN(SSTRING(name))
	#else
		//- TODO: Currently we have no way of setting the category of a function
		//------------------------------------------------------------------------------------------------------------------------
		#define CORE_ZONE core::profile::cpu::cscoped_function			\
		ANONYMOUS_VARIABLE(cpu_profile_function)						\
		(																\
			std::hash<std::thread::id>{}(std::this_thread::get_id()),	\
			CORE_FUNC_SIG,												\
			nullptr,													\
			__FILE__,													\
			__LINE__													\
		)
		//- Create a named zone
		//------------------------------------------------------------------------------------------------------------------------
		#define CORE_NAMED_ZONE(name) core::profile::cpu::cscoped_function			\
		ANONYMOUS_VARIABLE(cpu_profile_function)									\
		(																			\
			std::hash<std::thread::id>{}(std::this_thread::get_id()),				\
			SSTRING(name),															\
			nullptr,																\
			__FILE__,																\
			__LINE__																\
		)
	#endif
#else
	#define CORE_ZONE
	#define CORE_NAMED_ZONE(name)
#endif

namespace core
{
	class cmemory;
	class cargs;

} //- core

template<typename T>
using ref_t = std::shared_ptr<T>;

template<typename T>
using ptr_t = std::unique_ptr<T>;
using stringview_t = stl::string_view;
using string_t = stl::string;

template<typename T>
using queue_t = stl::queue<T>;

template<typename T>
using vector_t = stl::vector<T>;

template<typename K, typename T>
using umap_t = stl::unordered_map<K, T>;

template<typename T>
using uset_t = stl::unordered_set<T>;

template<typename K, typename T>
using map_t = stl::map<K, T>;

template<typename T, size_t S>
using array_t = stl::array<T, S>;

template<typename T>
using deque_t = stl::deque<T>;

template<size_t TSize>
using bitset_t = stl::bitset<TSize>;

template<typename T>
using list_t = stl::list<T>;

template<typename T>
using set_t = stl::set<T>;

//- defining handles here as they migh have to be registered in RTTR
//------------------------------------------------------------------------------------------------------------------------
using handle_type_t = uint64_t;
#define invalid_handle_t MAX(handle_type_t)
using service_type_t = handle_type_t;
using entity_proxy_t = int;
using query_t = handle_type_t;
using launch_context_t = int; //- enum core::launch_context

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

//------------------------------------------------------------------------------------------------------------------------
using byte_t = char;
using memory_ref_t = ref_t<core::cmemory>;
using args_ref_t = ref_t<core::cargs>;

namespace core
{
	template<typename T>
	using stack_t = stl::stack<T>;

	//------------------------------------------------------------------------------------------------------------------------
	enum launch_context : uint8_t
	{
		launch_context_none = 0,
		launch_context_client = BIT(1), //- Application launched in client mode
		launch_context_editor = BIT(2), //- Application launched in editor mode
		launch_context_server = BIT(3), //- Application launched in server mode
	};

	enum future_status : uint8_t
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
	enum file_mode : uint8_t
	{
		file_mode_none = 0,
		file_mode_read = BIT(1),
		file_mode_write = BIT(2),
		file_mode_read_write = file_mode_read | file_mode_write,
		file_mode_append = BIT(3),
		file_mode_truncate = BIT(4)
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum file_state : uint8_t
	{
		file_state_none = 0,
		file_state_opened = BIT(1),
		file_state_read_only = BIT(2),
		file_state_has_changes = BIT(3),
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum file_seek_origin : uint8_t
	{
		file_seek_origin_none = 0,
		file_seek_origin_begin,		//- position relative from start of file (i.e. forwards)
		file_seek_origin_end,		//- position relative from end of file (i.e. backwards)
		file_seek_origin_set		//- position relative from current position in file (i.e. relative to cursor in file)
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
		logging_verbosity_trace = SPDLOG_LEVEL_TRACE,	//- internal
		logging_verbosity_debug = SPDLOG_LEVEL_DEBUG,	//- internal
		logging_verbosity_info = SPDLOG_LEVEL_INFO,
		logging_verbosity_warn = SPDLOG_LEVEL_WARN,
		logging_verbosity_error = SPDLOG_LEVEL_ERROR,
		logging_verbosity_critical = SPDLOG_LEVEL_CRITICAL,
		logging_verbosity_off = SPDLOG_LEVEL_OFF,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum module_status : uint8_t
	{
		module_status_none = 0,
		module_status_unloaded,
		module_status_loading,
		module_status_ready,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum mouse_button : uint8_t
	{
		mouse_button_none = 0,
		mouse_button_left,
		mouse_button_right,
		mouse_button_middle,

		mouse_button_count
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum gamepad_axis : uint8_t
	{
		gamepad_axis_none = 0,
		gamepad_axis_left_x,
		gamepad_axis_left_y,
		gamepad_axis_left_z,
		gamepad_axis_right_x,
		gamepad_axis_right_y,
		gamepad_axis_right_z,

		gamepad_axis_count
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum key_modifier : uint8_t
	{
		key_modifier_none = 0,
		key_modifier_left_alt = BIT(0),
		key_modifier_right_alt = BIT(1),
		key_modifier_left_ctrl = BIT(2),
		key_modifier_right_ctrl = BIT(3),
		key_modifier_left_shift = BIT(4),
		key_modifier_right_shift = BIT(5),
		key_modifier_left_meta = BIT(6),
		key_modifier_right_meta = BIT(7),
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum key : int
	{
		key_none = 0,
		key_esc,
		key_return,
		key_tab,
		key_space,
		key_backspace,
		key_up,
		key_down,
		key_left,
		key_right,
		key_insert,
		key_delete,
		key_home,
		key_end,
		key_page_up,
		key_page_down,
		key_print,
		key_plus,
		key_minus,
		key_left_bracket,
		key_right_bracket,
		key_semicolon,
		key_quote,
		key_comma,
		key_period,
		key_slash,
		key_backslash,
		key_tilde,
		key_f1,
		key_f2,
		key_f3,
		key_f4,
		key_f5,
		key_f6,
		key_f7,
		key_f8,
		key_f9,
		key_f10,
		key_f11,
		key_f12,
		key_numpad0,
		key_numpad1,
		key_numpad2,
		key_numpad3,
		key_numpad4,
		key_numpad5,
		key_numpad6,
		key_numpad7,
		key_numpad8,
		key_numpad9,
		key_0,
		key_1,
		key_2,
		key_3,
		key_4,
		key_5,
		key_6,
		key_7,
		key_8,
		key_9,
		key_a,
		key_b,
		key_c,
		key_d,
		key_e,
		key_f,
		key_g,
		key_h,
		key_i,
		key_j,
		key_k,
		key_l,
		key_m,
		key_n,
		key_o,
		key_p,
		key_q,
		key_r,
		key_s,
		key_t,
		key_u,
		key_v,
		key_w,
		key_x,
		key_y,
		key_z,

		key_gamepad_a,
		key_gamepad_b,
		key_gamepad_x,
		key_gamepad_y,
		key_gamepad_thumb_L,
		key_gamepad_thumb_R,
		key_gamepad_shoulder_L,
		key_gamepad_shoulder_R,
		key_gamepad_up,
		key_gamepad_down,
		key_gamepad_left,
		key_gamepad_right,
		key_gamepad_back,
		key_gamepad_start,
		key_gamepad_guide,

		key_count
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

	//------------------------------------------------------------------------------------------------------------------------
	class cnon_movable
	{
	public:
		cnon_movable() = default;
		virtual ~cnon_movable() = default;
	private:
		cnon_movable(cnon_movable&&) = delete;
		cnon_movable& operator=(const cnon_movable&&) = delete;
	};

	using error_report_function_t = std::function<void(logging_verbosity, const string_t&)>;

	//------------------------------------------------------------------------------------------------------------------------
	struct serror_reporter
	{
		STATIC_INSTANCE_EX(serror_reporter);

		static void set_logger(error_report_function_t callback);

		error_report_function_t m_callback = nullptr;
	};

} //- core

namespace asio::detail
{
	//------------------------------------------------------------------------------------------------------------------------
	template <typename Exception>
	void throw_exception(const Exception& e ASIO_SOURCE_LOCATION_DEFAULTED_PARAM);

	//------------------------------------------------------------------------------------------------------------------------
	template <typename Exception>
	void throw_exception(const Exception& e ASIO_SOURCE_LOCATION_PARAM)
	{
		if (core::serror_reporter::instance().m_callback)
		{
			core::serror_reporter::instance().m_callback(core::logging_verbosity_critical,
				fmt::format("[ASIO] Exception occurred with message '{}'", e.what()));
		}
	}

} //- asio::detail
