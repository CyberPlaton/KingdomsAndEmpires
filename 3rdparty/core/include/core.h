#pragma once
#include "detail/core_platform.hpp"
#if defined(CORE_USE_EASTL)
#include <eastl.h>
namespace stl = eastl;
#else
#include <cstddef>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <string>
namespace stl = std;
#endif
#include <glm.h>
#include <rttr.h>
#include <magic_enum.h>
#include <taskflow.h>
#include <cassert>

#define STRINGIFY(s) #s
#define STRING(s) STRINGIFY(s)
#define SSTRING(s) STRING(s)

#define MAX(type) std::numeric_limits<type>().max()
#define MIN(type) std::numeric_limits<type>().min()
#define BIT(x) 1 << x
#define CORE_ASSERT(expression, message) assert((expression, message))
#define ASSERT(expression, ...) CORE_ASSERT(expression, __VA_ARGS__)
#define SCAST(type, value) static_cast<type>(value)
#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif
#define ARRAYSIZE(__array) ((u32)(sizeof(__array) / sizeof(*(__array))))
#define STATIC_INSTANCE(__class, __member) \
static __class& instance() \
{ static __class __member; return __member;}

template<class T>
using ref_t = stl::shared_ptr<T>;
template<class T>
using ptr_t = stl::unique_ptr<T>;
using string_t = stl::string;
using stringview_t = stl::string_view;
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
using technique_t = handle_type_t;
using texture_t = handle_type_t;
using uniform_t = handle_type_t;
using material_t = handle_type_t;
using spriteatlas_t = handle_type_t;
using subtexture_t = handle_type_t;

using ivec2_t = glm::lowp_u32vec2;
using vec2_t = glm::vec2;
using vec3_t = glm::vec3;
using vec4_t = glm::vec4;
using mat3_t = glm::mat3x3;
using mat4_t = glm::mat4x4;

namespace core
{
	//- forward declarations
	class cuuid;

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
	unsigned hash(const core::cuuid& uuid);
	//b2AABB create_aabb(const core::srect& rect);
	//b2AABB create_aabb(const vec2_t& center, const vec2_t& halfextents);
	//b2AABB create_aabb(f32 x, f32 y, f32 size);
	//b2AABB create_aabb(f32 x, f32 y, f32 w, f32 h);
	unsigned percentage(float total_value, float part_value);
	float percent_value(unsigned p, float total_value);
	bool is_valid_handle(handle_type_t h);
	handle_type_t invalid_handle();

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TType, typename TIterator>
	bool find(TIterator begin, TIterator end, const TType& value)
	{
		return stl::find(begin, end, value) != end;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TEnum>
	stringview_t enum_to_string(TEnum value)
	{
		return magic_enum::enum_name(value);
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

	//- RTTR aware replacement for std::pair<>
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TKey, typename TValue>
	struct spair
	{
		TKey first;
		TValue second;
	};

	//- define some common types of pairs
	//------------------------------------------------------------------------------------------------------------------------
	using smaterial_pair = spair<texture_t, material_t>;

	//------------------------------------------------------------------------------------------------------------------------
	class casync final
	{
	public:
		template<typename TCallable>
		static void launch_silent(TCallable&& function)
		{
			C_EXECUTOR.silent_async(function);
		}

		template<typename TCallable>
		static decltype(auto) launch_async(TCallable&& function)
		{
			return C_EXECUTOR.async(function);
		}

	private:
		static inline tf::Executor C_EXECUTOR;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cuuid final
	{
	public:
		cuuid();
		cuuid(const string_t& uuid);
		cuuid(size_t seed);
		cuuid(const cuuid& other);
		~cuuid() = default;

		inline stringview_t view() const { return m_string.data(); }
		inline const char* c_str() const { return m_string.c_str(); }
		inline unsigned hash() const { return m_hash; }
		inline bool is_equal_to(const cuuid& uuid) const { return compare(uuid) == 0; }
		inline bool is_smaller_as(const cuuid& uuid) const { return compare(uuid) < 0; }
		inline bool is_higher_as(const cuuid& uuid) const { return compare(uuid) > 0; }

	private:
		inline static const auto C_RANDOM_BYTES_COUNT = 4;
		inline static const unsigned char C_HEX[] = "0123456789abcdef";
		array_t<unsigned char, 16u> m_data;
		string_t m_string;
		unsigned m_hash;

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
		RTTR_REGISTRATION_FRIEND;
	};

	//- random number generator
	//------------------------------------------------------------------------------------------------------------------------
	class crandom
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
	class cmutex
	{
	public:
		cmutex();
		~cmutex();

		void lock();
		void unlock();

	private:
		__declspec(align(16)) uint8_t m_internal[64];
	};

} //- core