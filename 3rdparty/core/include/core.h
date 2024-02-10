#pragma once
#include <core/core_platform.hpp>
#if defined(CORE_USE_EASTL)
#include <eastl.h>
namespace stl = eastl;
#else
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <string>
namespace stl = std;
#endif
#include <mimalloc.h>
#include <glm.h>
#include <magic_enum.h>
#include <taskflow.h>
#include <spdlog.h>
#include <rttr.h>
#include <cstddef>
#include <memory>
#include <filesystem>
#include <random>
#include <cassert>
#include <future>
#include <chrono>
#include <any>
#include <new>
#include <fstream>

#if CORE_USE_EASTL
#define RAPIDJSON_HAS_STDSTRING 0
#else
#define RAPIDJSON_HAS_STDSTRING 1
#endif
#include <rapidjson.h>

#define STRINGIFY(s) #s
#define STRING(s) STRINGIFY(s)
#define SSTRING(s) STRING(s)

#define MAX(type) std::numeric_limits<type>().max()
#define MIN(type) std::numeric_limits<type>().min()
#define BIT(x) 1 << x

#ifdef DEBUG
#define CORE_ASSERT(expression, message) assert(expression && message)
#define ASSERT(expression, ...) CORE_ASSERT(expression, __VA_ARGS__)
#else
#define CORE_ASSERT(expression, message)
#define ASSERT(expression, ...)
#endif

#define SCAST(type, value) static_cast<type>(value)
#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif
#define ARRAYSIZE(__array) ((u32)(sizeof(__array) / sizeof(*(__array))))
#define STATIC_INSTANCE(__class, __member) static __class& instance() { static __class __member; return __member; }

#define CORE_MALLOC(size)		mi_malloc(size)
#define CORE_CALLOC(n, size)	mi_calloc(n, size)
#define CORE_REALLOC(p, size)	mi_realloc(p, size)
#define CORE_FREE(p)			mi_free(p)

template<class T>
using ref_t = stl::shared_ptr<T>;
template<class T>
using ptr_t = stl::unique_ptr<T>;
using string_t = stl::string;
using stringview_t = const char*;
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
template<class T, unsigned S>
using array_t = stl::array<T, S>;

using handle_type_t = uint16_t;
#define invalid_handle_t MAX(uint16_t);
using service_type_t = handle_type_t;
using technique_t = handle_type_t;
using texture_t = handle_type_t;
using uniform_t = handle_type_t;
using material_t = handle_type_t;
using spriteatlas_t = handle_type_t;
using subtexture_t = handle_type_t;
using render_target_t = handle_type_t;

using ivec2_t = glm::lowp_u32vec2;
using vec2_t = glm::vec2;
using vec3_t = glm::vec3;
using vec4_t = glm::vec4;
using mat3_t = glm::mat3x3;
using mat4_t = glm::mat4x4;

#if defined(core_EXPORTS) && defined(CORE_USE_EASTL)
//- implementation required for EASTL. The function will be available in any application or plugin
//- linking to core, the implementation however is only exported to static library.
//------------------------------------------------------------------------------------------------------------------------
void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return mi_malloc(size);
}

//- implementation required for EASTL. The function will be available in any application or plugin
//- linking to core, the implementation however is only exported to static library.
//------------------------------------------------------------------------------------------------------------------------
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags,
	const char* file, int line)
{
	return mi_aligned_alloc(alignment, size);
}
#endif

namespace core
{
	//- forward declarations
	class cuuid;
	struct srect;

	//- common enums etc.
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
	enum file_io_mode : uint8_t
	{
		file_io_mode_none		= 0,
		file_io_mode_read		= BIT(0),
		file_io_mode_write		= BIT(1),
		file_io_mode_override	= BIT(2),
		file_io_mode_truncate	= BIT(3),
		file_io_mode_append		= BIT(4),
		file_io_mode_binary		= BIT(5),
		file_io_mode_text		= BIT(6),


		file_io_mode_read_text = file_io_mode_read | file_io_mode_text,
		file_io_mode_read_bin = file_io_mode_read | file_io_mode_binary,
		file_io_mode_write_text = file_io_mode_write | file_io_mode_text,
		file_io_mode_write_bin = file_io_mode_write | file_io_mode_binary,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum common_color
	{
		//- convention: 400 - dark, 200 - normal, 50 - light
		common_color_red400,
		common_color_red200,
		common_color_red50,

		common_color_orange400,
		common_color_orange200,
		common_color_orange50,

		common_color_yellow400,
		common_color_yellow200,
		common_color_yellow50,

		common_color_green400,
		common_color_green200,
		common_color_green50,

		common_color_cyan400,
		common_color_cyan200,
		common_color_cyan50,

		common_color_blue400,
		common_color_blue200,
		common_color_blue50,

		common_color_magenta400,
		common_color_magenta200,
		common_color_magenta50,

		common_color_pink400,
		common_color_pink200,
		common_color_pink50,

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

	//- RTTR aware replacement for std::pair<>
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TKey, typename TValue>
	struct spair
	{
		TKey first;
		TValue second;

		RTTR_ENABLE();
	};

} //- core

namespace math
{

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType>
	bool almost_equal(TType x, TType y)
	{
		return glm::distance(glm::abs<TType>(x), glm::abs<TType>(y)) < glm::epsilon<TType>();
	}

} //- math

namespace algorithm
{
	vec3_t decompose_rotation(const mat4_t& transform);
	vec3_t decompose_translation(const mat4_t& transform);
	vec3_t decompose_scale(const mat4_t& transform);
	void decompose_rotation(const mat4_t& transform, vec3_t& out);
	void decompose_translation(const mat4_t& transform, vec3_t& out);
	void decompose_scale(const mat4_t& transform, vec3_t& out);
	unsigned hash(stringview_t string);
	unsigned percentage(float total_value, float part_value);
	float percent_value(unsigned p, float total_value);
	bool is_valid_handle(handle_type_t h);
	handle_type_t invalid_handle();

	//- utility to check whether query number is bitwise active in value
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TEnum>
	bool bit_on(int value, TEnum query)
	{
		return (value & query);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType, typename TIterator>
	bool find(TIterator begin, TIterator end, const TType& value)
	{
		return stl::find(begin, end, value) != end;
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
	template<typename TStructure>
	void erase_at_index(TStructure& structure, unsigned index)
	{
		structure.erase(structure.begin() + index);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TStructure, typename TType>
	void erase_first(TStructure& structure, TType value)
	{
		structure.erase(value);
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

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TIterator>
	void shuffle(TIterator begin, TIterator end, unsigned seed = time(NULL))
	{
		static const std::default_random_engine C_RANDOM_ENGINE(seed);

		stl::shuffle(begin, end, C_RANDOM_ENGINE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TIterator, typename TCallable>
	void sort(TIterator begin, TIterator end, TCallable&& function)
	{
		stl::sort(begin, end, function);
	}

} //- algorithm

namespace io
{
	string_t to_json(rttr::instance object);
	bool from_json(const string_t& json, rttr::instance object);

} //- io

namespace engine
{
	class cservice_manager;

} //- engine

namespace core
{

	namespace string_utils
	{
		void split(const string_t& string, char delimiter, stl::vector< string_t >& storage);
		void insert(string_t& string, const string_t& to_insert_one, size_t index);
		void push_front(string_t& string, const string_t& to_prepend_one);
		void push_back(string_t& string, const string_t& to_append_one);
		void erase_substr(string_t& string, const string_t& substring_to_erase);
		void erase_range(string_t& string, size_t begin, size_t end);
		void to_lower(string_t& string);
		size_t length(const string_t& string);
		size_t find_substr(const string_t& string, const string_t& substring);
		bool does_substr_exist(const string_t& string, const string_t& substring);
		bool compare(const string_t& first, const string_t& second);

	} //- string_utils

	namespace detail
	{

	} //- detail

	//- define some common types of pairs
	//------------------------------------------------------------------------------------------------------------------------
	using smaterial_pair = spair<texture_t, material_t>;

	//- base class for a service
	//------------------------------------------------------------------------------------------------------------------------
	class cservice
	{
		friend class engine::cservice_manager;
	public:
		inline static const service_type_t C_INVALID_TYPE = invalid_handle_t;
		inline static const unsigned C_SERVICE_COUNT_MAX = 64;

		cservice() = default;
		virtual ~cservice() = default;

		virtual bool on_start() { return false; }
		virtual void on_shutdown() {}
		virtual void on_update(float) {}

		RTTR_ENABLE();
	};

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

		cuuid();
		cuuid(const string_t& uuid);
		cuuid(size_t seed);
		cuuid(const cuuid& other);
		~cuuid() = default;

		inline stringview_t view() const { return m_string.data(); }
		inline const char* c_str() const { return m_string.c_str(); }
		inline unsigned hash() const { return algorithm::hash(view()); }
		inline bool is_equal_to(const cuuid& uuid) const { return compare(uuid) == 0; }
		inline bool is_smaller_as(const cuuid& uuid) const { return compare(uuid) < 0; }
		inline bool is_higher_as(const cuuid& uuid) const { return compare(uuid) > 0; }

	private:
		inline static const auto C_RANDOM_BYTES_COUNT = 4;
		inline static const unsigned char C_HEX[] = "0123456789abcdef";

		string_t m_string;
		array_t<unsigned char, 16u> m_data;

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
		REFLECTABLE();
	};

	//- random number generator
	//------------------------------------------------------------------------------------------------------------------------
	class crandom final
	{
	public:
		crandom() = default;
		~crandom() = default;

		static void seed(unsigned value = 0);

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
		static inline std::mt19937 C_RANDOM_ENGINE;
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

		vec4_t normalize() const;

		uint8_t m_r, m_g, m_b, m_a;

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct srect final
	{
		srect(const vec2_t& xy, const vec2_t& wh);
		srect(float x = 0.0f, float y = 0.0f, float w = 0.0f, float h = 0.0f);

		inline float x() const { return m_x; };
		inline float y() const { return m_y; };
		inline float w() const { return m_w; };
		inline float h() const { return m_h; };

		inline vec2_t top_left() const { return {m_x, m_y}; }
		inline vec2_t top_right() const { return { m_x + m_w, m_y }; }
		inline vec2_t bottom_left() const { return { m_x, m_y + m_h }; }
		inline vec2_t bottom_right() const { return { m_x + m_w, m_y + m_h }; }

		void set(float x, float y, float w, float h);
		void position(float x, float y);
		void dimension(float w, float h);

		float m_x, m_y, m_w, m_h;

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cnon_copyable
	{
	public:
		cnon_copyable() = default;
		~cnon_copyable() = default;
	private:
		cnon_copyable& operator=(const cnon_copyable&) = delete;
		cnon_copyable(const cnon_copyable&) = delete;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cpath final
	{
	public:
		cpath(stringview_t path);
		cpath(const std::filesystem::path& path);
		cpath(const cpath& path);
		cpath& operator=(const cpath& path);
		~cpath() = default;

		std::filesystem::path path() const;
		std::filesystem::directory_entry dir() const;

		inline stringview_t view() const { return m_string_path.c_str(); }
		inline stringview_t extension() const { return m_string_ext.c_str(); }
		inline stringview_t stem() const { return m_string_stem.c_str(); }

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
		~cfilesystem() = default;

		static cpath construct(stringview_t path, stringview_t addition);
		static cpath join(stringview_t path, stringview_t addition);
		static cpath cwd();

		static bool create_dir(stringview_t path);
		static bool create_dir_in(stringview_t path, stringview_t name);
		static bool create_dir_recursive(stringview_t path);

		static bool create_file(stringview_t path);
		static bool create_file_in(stringview_t path, stringview_t stem, stringview_t ext);

		static bool remove(stringview_t path);
		static bool remove(stringview_t path, stringview_t name);

		static bool find_file(stringview_t path, stringview_t name);
		static bool find_file_by_stem(stringview_t path, stringview_t name);
		static bool find_file_by_extension(stringview_t path, stringview_t name);
		static bool find_dir(stringview_t path, stringview_t name);
		static bool find_at(stringview_t path, stringview_t name, filesystem_lookup_type type);

		static cpath construct_relative_to_cwd(stringview_t path);
		static bool is_contained(const cpath& contained, const cpath& container);

		inline cpath current() const { return m_current; }

		bool forwards(stringview_t path, bool forced = false);
		bool backwards();

		void append(stringview_t path);
		cfilesystem& operator/=(stringview_t path);

		inline bool exists() const { return m_current.exists(); };
		inline bool is_file() const { return m_current.is_file(); };
		inline bool is_dir() const { return m_current.is_dir(); };

	private:
		cpath m_current;
	};

	//- Helper class for performing I/O operations both sync and async.
	//- Data is automatically freed when object goes out of scope.
	//- Object wíll not go out of scope until async task is finished.
	//------------------------------------------------------------------------------------------------------------------------
	class cfile final
	{
	public:
		static string_t load_text(stringview_t path);

		static std::future<string_t> load_text_async(stringview_t path);

		static file_io_status save_text(stringview_t path, const string_t& text);

		static std::future<file_io_status> save_text_async(stringview_t path, const string_t& text);

		static spair<uint8_t*, unsigned> load_binary(stringview_t path);

		static std::future<spair<uint8_t*, unsigned>> load_binary_async(stringview_t path);

		static file_io_status save_binary(stringview_t path, uint8_t* data, unsigned size);

		static std::future<file_io_status> save_binary_async(stringview_t path, uint8_t* data, unsigned size);

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

	//------------------------------------------------------------------------------------------------------------------------
	class cany final
	{
	public:
		cany() = default;
		template<typename TType>
		cany(TType&& data) :
			m_data(data)
		{
		}
		template<typename TType>
		cany(const TType& data) :
			m_data(data)
		{
		}
		template<typename TType>
		cany(TType data) :
			m_data(data)
		{
		}
		cany& operator=(const cany& other);
		~cany();

		inline decltype(auto) type() const { return m_data.type().hash_code(); }
		inline stringview_t type_name() const { return m_data.type().name(); }
		inline bool empty() const { return !m_data.has_value(); }

		template<typename TType>
		inline bool is() const
		{
			return typeid(TType).hash_code() == type();
		}

		template<typename TType>
		TType cast() const noexcept
		{
			ASSERT(is<TType>(), "Casting to another data type is not allowed");

			return std::any_cast<TType>(m_data);
		}

		template<typename TType>
		TType& cast_ref() noexcept
		{
			ASSERT(is<TType>(), "Casting to another data type is not allowed");

			return std::any_cast<TType&>(m_data);
		}

		template<typename TType>
		const TType& cast_ref() const noexcept
		{
			ASSERT(is<TType>(), "Casting to another data type is not allowed");

			return std::any_cast<TType&>(m_data);
		}

	private:
		std::any m_data;
	};

} //- core

namespace core
{
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
			.property("m_data", &cuuid::m_data)
			.property("m_string", &cuuid::m_string)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(scolor)
	{
		rttr::registration::class_<scolor>("scolor")
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
			.property("x", &vec2_t::x)
			.property("y", &vec2_t::y)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(vec3_t)
	{
		rttr::registration::class_<vec3_t>("vec3_t")
			.property("x", &vec3_t::x)
			.property("y", &vec3_t::y)
			.property("z", &vec3_t::z)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(vec4_t)
	{
		rttr::registration::class_<vec4_t>("vec4_t")
			.property("x", &vec4_t::x)
			.property("y", &vec4_t::y)
			.property("z", &vec4_t::z)
			.property("w", &vec4_t::w)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(smaterial_pair)
	{
		rttr::registration::class_<smaterial_pair>("smaterial_pair")
			.property("first", &smaterial_pair::first)
			.property("second", &smaterial_pair::second)
			;
	}

} //- core