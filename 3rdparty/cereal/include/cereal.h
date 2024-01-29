#pragma once
#define CEREAL_LOAD_FUNCTION_NAME load
#define CEREAL_SAVE_FUNCTION_NAME save
#define CEREAL_SERIALIZE_FUNCTION_NAME serialize
#include "cereal/cereal.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/archives/portable_binary.hpp"
#include "cereal/archives/xml.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/array.hpp"
#include "cereal/types/string.hpp"