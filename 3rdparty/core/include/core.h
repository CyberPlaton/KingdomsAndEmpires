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

#include <rttr.h>

namespace algorithm
{


} //- algorithm

namespace core
{
	namespace string_utils
	{

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

		stringview_t view() const;
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