#include "core_io.hpp"
#include "core_algorithm.hpp"
#include "core_filesystem.hpp"
#include "core_memory.hpp"

namespace core
{
	namespace io
	{
		bool write_variant(const rttr::variant& var, nlohmann::json& json);
		rttr::variant from_json_recursively(rttr::type expected, const simdjson::dom::element& json);
		void from_json_recursively(rttr::variant& object_out, rttr::type expected, const simdjson::dom::element& json);

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TNumeric>
		rttr::variant extract_numeric_type(TNumeric value, rttr::type required_type)
		{
			if (rttr::type::get<float>() == required_type) return static_cast<float>(value);
			else if (rttr::type::get<double>() == required_type) return static_cast<double>(value);
			else if (rttr::type::get<int64_t>() == required_type) return static_cast<int64_t>(value);
			else if (rttr::type::get<int32_t>() == required_type) return static_cast<int32_t>(value);
			else if (rttr::type::get<int16_t>() == required_type) return static_cast<int16_t>(value);
			else if (rttr::type::get<int8_t>() == required_type) return static_cast<int8_t>(value);
			else if (rttr::type::get<uint64_t>() == required_type) return static_cast<uint64_t>(value);
			else if (rttr::type::get<uint32_t>() == required_type) return static_cast<uint32_t>(value);
			else if (rttr::type::get<uint16_t>() == required_type) return static_cast<uint16_t>(value);
			else if (rttr::type::get<uint8_t>() == required_type) return static_cast<uint8_t>(value);
			else if (rttr::type::get<char>() == required_type) return static_cast<char>(value);
			else if (rttr::type::get<unsigned char>() == required_type) return static_cast<unsigned char>(value);
			else if (rttr::type::get<bool>() == required_type) return static_cast<bool>(value);

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_error,
					fmt::format("\tCould not extract numeric type '{}' from number '{} ({})'",
						required_type.get_name().data(), value, rttr::type::get<TNumeric>().get_name().data()));
			}
			return value;
		}

		//------------------------------------------------------------------------------------------------------------------------
		simdjson::dom::element_type extract_type(rttr::type type)
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_warn,
					fmt::format("\tExtracting RTTR type '{}'", type.get_name().data()));
			}

			if (rttr::type::get<double>() == type ||
				rttr::type::get<float>() == type)
			{
				return simdjson::dom::element_type::DOUBLE;
			}
			else if (rttr::type::get<int64_t>() == type ||
				rttr::type::get<int32_t>() == type ||
				rttr::type::get<int16_t>() == type ||
				rttr::type::get<int8_t>() == type)
			{
				return simdjson::dom::element_type::INT64;
			}
			else if (rttr::type::get<uint64_t>() == type ||
				rttr::type::get<uint32_t>() == type ||
				rttr::type::get<uint16_t>() == type ||
				rttr::type::get<uint8_t>() == type)
			{
				return simdjson::dom::element_type::UINT64;
			}
			else if (rttr::type::get<bool>() == type)
			{
				return simdjson::dom::element_type::BOOL;
			}
			else if (rttr::type::get<string_t>() == type ||
				rttr::type::get<stringview_t>() == type ||
				rttr::type::get<const char*>() == type)
			{
				return simdjson::dom::element_type::STRING;
			}
			else if (type.is_class() || type.is_wrapper())
			{
				return simdjson::dom::element_type::OBJECT;
			}
			else if (type.is_sequential_container() || type.is_associative_container())
			{
				return simdjson::dom::element_type::ARRAY;
			}
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_error, fmt::format("\t\tUnrecognized RTTR type '{}'",
					type.get_name().data()));
			}
			return simdjson::dom::element_type::NULL_VALUE;
		}

		//------------------------------------------------------------------------------------------------------------------------
		rttr::variant construct_object(rttr::type type)
		{
			if (type.get_constructor().is_valid())
			{
				return type.get_constructor().invoke();
			}
			else if (auto var = type.create(); var.is_valid())
			{
				return var;
			}
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_error, fmt::format("No default constructor found for RTTR type '{}'",
					type.get_name().data()));
			}
			return rttr::variant();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void extract_from_array(const rttr::variant& object, const simdjson::dom::array& json)
		{
			auto type = object.get_type();

			//- vector/array
			if (type.is_sequential_container())
			{
				auto view = object.create_sequential_view();
				auto expected = view.get_value_type();

				view.set_size(json.size());

				auto i = 0;
				for (auto it : json)
				{
					auto val = from_json_recursively(expected, it);

					if (!view.set_value(i++, std::move(val)) && serror_reporter::instance().m_callback)
					{
						serror_reporter::instance().m_callback(logging_verbosity_error,
							fmt::format("\tCould not set sequential array value with type '{}' at index '{}'",
								type.get_name().data(), i));
					}
				}
			}
			//- map
			else if (type.is_associative_container())
			{
				auto view = object.create_associative_view();
				auto key_expected = view.get_key_type();
				auto val_expected = view.get_value_type();

				for (auto it : json)
				{
					simdjson::dom::element key_elem, val_elem;

					if (it.at_key(C_MAP_KEY_NAME).get(key_elem) != simdjson::SUCCESS ||
						it.at_key(C_MAP_VALUE_NAME).get(val_elem) != simdjson::SUCCESS)
					{
						continue;
					}

					auto key = from_json_recursively(key_expected, key_elem);
					auto val = from_json_recursively(val_expected, val_elem);

					if (!view.insert(key, val).second && serror_reporter::instance().m_callback)
					{
						serror_reporter::instance().m_callback(logging_verbosity_error,
							fmt::format("\tCould not set associative array value with type '<{}:{}>'",
								key_expected.get_name().data(), val_expected.get_name().data()));
					}
				}
			}
			else
			{
				//- error
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		rttr::variant extract_from_string(stringview_t value, const rttr::type& type)
		{
			rttr::variant var = string_t(value);

			if (var.get_type() == type || rttr::type::get<stringview_t>() == type)
			{
				return var;
			}
			//- try converting to desired type
			if (var.convert(type))
			{
				return var;
			}
			//- error
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_error,
					fmt::format("\tFailed extracting String '{}' to type '{}'",
						value.data(), type.get_name().data()));
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void extract_from_object(rttr::variant& object_out, const simdjson::dom::object& json)
		{
			auto type = object_out.get_type();

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_trace,
					fmt::format("Extracting RTTR type '{}'from JSON Object", type.get_name().data()));
			}

			if (type.is_class())
			{
				for (const auto& prop : type.get_properties())
				{
					auto name = prop.get_name().data();
					auto prop_type = prop.get_type().get_name().data();

					if (serror_reporter::instance().m_callback)
					{
						serror_reporter::instance().m_callback(logging_verbosity_trace,
							fmt::format("\tDeserializing property '{}' with type '{}'", name, prop_type));
					}

					simdjson::dom::element element;

					if (json.at_key(name).get(element) != simdjson::SUCCESS)
					{
						if (serror_reporter::instance().m_callback)
						{
							serror_reporter::instance().m_callback(logging_verbosity_warn,
								fmt::format("\t\tproperty '{}' with type '{}' not found", name, prop_type));
						}
						continue;
					}

					auto object = from_json_recursively(prop.get_type(), element);

					if (!prop.set_value(object_out, object) && serror_reporter::instance().m_callback)
					{
						serror_reporter::instance().m_callback(logging_verbosity_error,
							fmt::format("\tFailed setting value of from JSON element of type '{}' for property '{}::{} ({})'",
								algorithm::enum_to_string(element.type()), type.get_name().data(), name, prop.get_type().get_name().data()));
					}
				}
			}
			else
			{
				//- error
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_error,
						fmt::format("\tRTTR type '{}' is not a class type", type.get_name().data()));
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		rttr::variant from_json_recursively(rttr::type expected, const simdjson::dom::element& json)
		{
			rttr::variant var;
			from_json_recursively(var, expected, json);
			return var;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void from_json_recursively(rttr::variant& object_out, rttr::type expected, const simdjson::dom::element& json)
		{
			if (json.is_null())
			{
				object_out = {};
				return;
			}

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_trace,
					fmt::format("Deserializing JSON object into expected RTTR type '{}', with JSON type '{}'",
						expected.get_name().data(), algorithm::enum_to_string(json.type())));
			}

			switch (json.type())
			{
			case simdjson::dom::element_type::OBJECT:
			{
				if (!object_out)
				{
					object_out = construct_object(expected);
				}
				extract_from_object(object_out, json.get<simdjson::dom::object>());
				break;
			}
			case simdjson::dom::element_type::ARRAY:
			{
				if (!object_out)
				{
					object_out = construct_object(expected);
				}
				extract_from_array(object_out, json.get<simdjson::dom::array>());
				break;
			}
			case simdjson::dom::element_type::STRING:
			{
				if (expected.is_enumeration() && serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_trace,
						fmt::format("\tExtracting stringified enum of type '{}'",
							expected.get_name().data()));
				}

				object_out = extract_from_string(json.get_string().value(), expected);
				break;
			}
			case simdjson::dom::element_type::BOOL:
			{
				object_out = extract_numeric_type(json.get_bool().value(), expected);
				break;
			}
			case simdjson::dom::element_type::DOUBLE:
			{
				object_out = extract_numeric_type(json.get_double().value(), expected);
				break;
			}
			case simdjson::dom::element_type::INT64:
			{
				object_out = extract_numeric_type(json.get_int64().value(), expected);
				break;
			}
			case simdjson::dom::element_type::UINT64:
			{
				object_out = extract_numeric_type(json.get_uint64().value(), expected);
				break;
			}
			default:
			{
				break;
			}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		rttr::variant from_json_blob(rttr::type expected, const byte_t* data, unsigned size)
		{
			simdjson::dom::parser parser;
			simdjson::dom::element element;

			auto json = stringview_t(data);

			if (parser.parse(json.data(), json.length()).get(element) != simdjson::SUCCESS)
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_warn,
						fmt::format("Failed deserializing from json string with expected type '{}'", expected.get_name().data()));
				}
			}

			return from_json_object(expected, element);
		}

		//------------------------------------------------------------------------------------------------------------------------
		rttr::variant from_json_object(rttr::type expected, const simdjson::dom::element& json)
		{
			rttr::variant object;
			from_json_recursively(object, expected, json);
			return object;
		}

		//------------------------------------------------------------------------------------------------------------------------
		rttr::variant from_json_string(rttr::type expected, stringview_t json)
		{
			return from_json_blob(expected, (const byte_t*)json.data(), json.length());
		}

		//------------------------------------------------------------------------------------------------------------------------
		void to_json_recursive(const rttr::instance& object, nlohmann::json& json)
		{
			auto inst = object.get_type().get_raw_type().is_wrapper() ? object.get_wrapped_instance() : object;

			//-- wrap into object with type information
			//-- Note: type information is outside of the object to make deserializing easiert
			json[C_OBJECT_TYPE_NAME] = object.get_type().get_raw_type().get_name().data();
			json = nlohmann::json::object();

			for (const auto prop : inst.get_derived_type().get_properties())
			{
				auto val = prop.get_value(inst);
				auto name = prop.get_name();

				if (val && !prop.get_metadata(C_NO_SERIALIZE_META))
				{
					write_variant(val, json[name.data()]);
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool write_fundamental_type(const rttr::type& type, const rttr::variant& var, nlohmann::json& json)
		{
			if (type.is_arithmetic())
			{
				if (rttr::type::get<float>() == type) json = var.to_double();
				else if (rttr::type::get<double>() == type) json = var.to_double();
				else if (rttr::type::get<int64_t>() == type) json = var.to_int();
				else if (rttr::type::get<int32_t>() == type) json = var.to_int();
				else if (rttr::type::get<int16_t>() == type) json = var.to_int();
				else if (rttr::type::get<int8_t>() == type) json = var.to_int();
				else if (rttr::type::get<uint64_t>() == type) json = var.to_uint64();
				else if (rttr::type::get<uint32_t>() == type) json = var.to_uint64();
				else if (rttr::type::get<uint16_t>() == type) json = var.to_uint64();
				else if (rttr::type::get<uint8_t>() == type) json = var.to_uint64();
				else if (rttr::type::get<char>() == type) json = var.to_bool();
				else if (rttr::type::get<bool>() == type) json = var.to_bool();
				else
				{
					if (serror_reporter::instance().m_callback)
					{
						serror_reporter::instance().m_callback(logging_verbosity_error,
							fmt::format("Failed serializing unknown numeric type '{}'", type.get_name().data()));
					}
					return false;
				}
				return true;
			}
			else if (type.is_enumeration())
			{
				//- try serializing as string
				bool result = false;
				if (auto s = var.to_string(&result); result)
				{
					json = s;
					return true;
				}
				//- could not write enum value, ignore but report
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_warn,
						fmt::format("Failed serializing enum value with type '{}'", type.get_name().data()));
				}
				json = nullptr;
				return true;
			}
			else if (rttr::type::get<string_t>() == type ||
				rttr::type::get<stringview_t>() == type ||
				rttr::type::get<const char*>() == type)
			{
				json = var.to_string();
				return true;
			}
			else
			{
				//- Not quite an error, as we determine what to do if something is not a primitive
				//- but just in case report quietly
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_trace,
						fmt::format("Not processing unknown (fundamental) type '{}'", type.get_name().data()));
				}
			}
			return false;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void write_array(const rttr::variant_sequential_view& view, nlohmann::json& json)
		{
			json = nlohmann::json::array();

			auto i = 0;
			for (const auto& var : view)
			{
				write_variant(var, json[i++]);
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void write_map(const rttr::variant_associative_view& view, nlohmann::json& json)
		{
			json = nlohmann::json::array();

			auto i = 0;
			if (!view.is_key_only_type())
			{
				for (const auto& pair : view)
				{
					write_variant(pair.first, json[i][C_MAP_KEY_NAME.data()]);
					write_variant(pair.first, json[i][C_MAP_VALUE_NAME.data()]);
					++i;
				}
			}
			else
			{
				for (const auto& var : view)
				{
					write_variant(var.first, json[i++]);
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool write_variant(const rttr::variant& var, nlohmann::json& json)
		{
			auto v = var;
			auto t = var.get_type();

			if (t.is_wrapper())
			{
				//- extract wrapped type
				t = t.get_wrapped_type();
				v = v.extract_wrapped_value();
			}

			//- number/bool/string/enum
			if (write_fundamental_type(t, v, json))
			{
				return true;
			}
			//- vector/array
			else if (t.is_sequential_container())
			{
				write_array(v.create_sequential_view(), json);
			}
			//- map
			else if (t.is_associative_container())
			{
				write_map(v.create_associative_view(), json);
			}
			else
			{
				//- write an object
				if (auto props = t.get_properties(); !props.empty())
				{
					to_json_recursive(v, json);
					return true;
				}

				//- error
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_error,
						fmt::format("Failed serializing type '{}'", t.get_name().data()));
				}
				return false;
			}
			return true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		string_t to_json_string(rttr::instance object, bool beautify /*= false*/)
		{
			auto json = to_json_object(object);

			return json.dump(beautify ? 4 : -1);
		}

		//------------------------------------------------------------------------------------------------------------------------
		nlohmann::json to_json_object(rttr::instance object)
		{
			nlohmann::json json;

			if (object.is_valid())
			{
				to_json_recursive(object, json);
			}

			return std::move(json);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void to_json_object(rttr::instance object, nlohmann::json& json)
		{
			if (object.is_valid())
			{
				to_json_recursive(object, json);
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		simdjson::dom::element load_json(stringview_t path)
		{
			simdjson::dom::parser parser;
			simdjson::dom::element element;

			auto memory = fs::load_text_from_file(path);

			if (parser.parse((const char*)memory->data(), (size_t)memory->size()).get(element) == simdjson::SUCCESS)
			{
				return element;
			}
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_warn,
					fmt::format("Failed loading JSON file '{}'", path.data()));
			}
			return {};
		}

	} //- io

} //- core