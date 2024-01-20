#pragma once

#define STRINGIFY(s) #s
#define STRING(s) STRINGIFY(s)
#define SSTRING(s) STRING(s)
#define CORE_PLATFORM_WINDOWS 0
#define CORE_PLATFORM_LINUX 0
#define CORE_PLATFORM_OSX 0
#define CORE_PLATFORM_ANDROID 0
#define CORE_PLATFORM_IOS 0
#define CORE_PLATFORM_NX 0
#define CORE_PLATFORM_XBOXONE 0
#define CORE_PLATFORM_PS4 0
#define CORE_PLATFORM_PS5 0
#define CORE_PLATFORM_DESKTOP 0
#define CORE_PLATFORM_MOBILE 0
#define CORE_PLATFORM_CONSOLE 0
#define CORE_COMPILER_MSVC 0
#define CORE_COMPILER_CLANG 0
#define CORE_COMPILER_GCC 0
#define CORE_ARCH_64BIT 0
#define CORE_ARCH_32BIT 0

#define MAX(type) std::numeric_limits<type>().max()
#define MIN(type) std::numeric_limits<type>().min()
#define BIT(x) 1 << x
#include <cassert>
#define CORE_ASSERT(expression, message) assert((expression, message))
#define ASSERT(expression, ...) CORE_ASSERT(expression, __VA_ARGS__)
#define SCAST(type, value) static_cast<type>(value)
#define ARRAYSIZE(__array) ((u32)(sizeof(__array) / sizeof(*(__array))))
#define STATIC_INSTANCE(__class, __member) \
static __class& instance() \
{ static __class __member; return __member;}

#include <cstddef>
#include <memory>
template<class T>
using ref_t = std::shared_ptr<T>;
template<class T>
using ptr_t = std::unique_ptr<T>;
#include <string>
using string_t = std::string;
using stringview_t = std::string_view;
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <glm.h>
#include <rttr.h>
template<class T>
using queue_t = std::queue<T>;
template<class T>
using vector_t = std::vector<T>;
template<class K, class T>
using umap_t = std::unordered_map<K, T>;
template<class T>
using uset_t = std::unordered_set<T>;
template<class K, class T>
using map_t = std::map<K, T>;
template<class T, unsigned S>
using array_t = std::array<T, S>;

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

} //- algorithm

namespace core
{

	namespace string_utils
	{

		void split(const string_t& string, char delimiter, std::vector< string_t >& storage);
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

	//------------------------------------------------------------------------------------------------------------------------
	class cuuid
	{
	public:
		cuuid();
		cuuid(const string_t& uuid);
		cuuid(size_t seed);
		cuuid(const cuuid& other);
		~cuuid() = default;

		inline stringview_t view() const { return m_string.data(); }
		inline const char* c_str() const { return m_string.c_str(); }

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

} //- core