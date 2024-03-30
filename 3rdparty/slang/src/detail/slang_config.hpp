#pragma once
#include "platform.hpp"
#include <string>
#include <cstdint>
#include <variant>
#include <any>

//- If overriding STL classes define the namespace they are coming from same as below
//------------------------------------------------------------------------------------------------------------------------
#ifndef SLANG_CUSTOM_STL
#include "3rdparty/TINYSTL/vector.h"
#include "3rdparty/TINYSTL/unordered_map.h"
#include "3rdparty/TINYSTL/string.h"
namespace stl = tinystl;
#endif

namespace slang
{
	template<typename T, typename TAllocator>
	using vector_t = stl::vector<T, TAllocator>;

	template<typename TKey, typename TValue, typename TAllocator>
	using umap_t = stl::unordered_map<TKey, TValue, TAllocator>;

	using string_t = stl::string;

} //- slang