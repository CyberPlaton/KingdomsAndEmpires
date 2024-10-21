#pragma once
#include "core_config.hpp"

namespace core
{
	namespace io
	{
		constexpr stringview_t C_NO_SERIALIZE_META = "NO_SERIALIZE";
		constexpr stringview_t C_OBJECT_TYPE_NAME = "__type__";
		constexpr stringview_t C_MAP_KEY_NAME = "__key__";
		constexpr stringview_t C_MAP_VALUE_NAME = "__value__";

		[[nodiscard]] rttr::variant from_json_blob(rttr::type expected, const byte_t* data, unsigned size);
		[[nodiscard]] rttr::variant from_json_object(rttr::type expected, const simdjson::dom::element& json);
		[[nodiscard]] rttr::variant from_json_string(rttr::type expected, stringview_t json);
		string_t to_json_string(rttr::instance object, bool beautify = false);
		[[nodiscard]] nlohmann::json to_json_object(rttr::instance object);
		void to_json_object(rttr::instance object, nlohmann::json& json);
		[[nodiscard]] simdjson::dom::element load_json(stringview_t path);

	} //- io

} //- core