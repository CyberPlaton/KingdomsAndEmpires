#include "core.hpp"
#include <sstream>

namespace algorithm
{
	//------------------------------------------------------------------------------------------------------------------------
	unsigned hash(const char* string)
	{
		unsigned len = SCAST(unsigned, strlen(string));
		const char* s = string;
		unsigned h = len;
		for (auto i = 0u; i < len; ++s, ++i)
		{
			h = ((h << 5) + h) + (*s);
		}
		return h;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned hash(stringview_t string)
	{
		return hash(string.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned hash(const string_t& string)
	{
		return hash(string.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned percentage(float total_value, float part_value)
	{
		return SCAST(unsigned, (part_value / total_value) * 100);
	}

	//------------------------------------------------------------------------------------------------------------------------
	float percent_value(unsigned p, float total_value)
	{
		if (p < 1) p = 1;
		if (p > 100) p = 100;
		return total_value * (SCAST(float, p) / 100.0f);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid_handle(handle_type_t h)
	{
		return h != MAX(handle_type_t);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid_proxy(entity_proxy_t h)
	{
		return h >= 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	handle_type_t invalid_handle()
	{
		return MAX(handle_type_t);
	}

	//------------------------------------------------------------------------------------------------------------------------
	float bytes_to_kilobytes(unsigned b)
	{
		return SCAST(float, b) / 1000.0f;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float bytes_to_megabytes(unsigned b)
	{
		return bytes_to_kilobytes(b) / 1000.0f;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float bytes_to_gigabytes(unsigned b)
	{
		return bytes_to_megabytes(b) / 1000.0f;
	}

} //- algorithm

namespace core
{
	namespace
	{
		constexpr stringview_t C_EMPTY_STRING					= "";
		constexpr auto C_FILESYSTEMS_RESERVE_COUNT				= 16;
		constexpr auto C_FILESYSTEM_OPENED_FILES_RESERVE_COUNT	= 256;

		//------------------------------------------------------------------------------------------------------------------------
		static bool is_path_directory(stringview_t path)
		{
			return string_utils::find_substr(path.data(), ".") == MAX(size_t);
		}

		//------------------------------------------------------------------------------------------------------------------------
		static bool is_path_file(stringview_t path)
		{
			return string_utils::find_substr(path.data(), ".") != MAX(size_t);
		}

		//- @reference: raylib UnloadFileData.
		//- unload data allocated by load_binary_file_data
		//------------------------------------------------------------------------------------------------------------------------
		static void unload_file_binary_data(void* data)
		{
			CORE_FREE(data);
		}

		//- @reference: raylib LoadFileData. Sevure version. If 'error' is not null then it will be filled with an error message.
		//- Does append a null terminator at the end.
		//------------------------------------------------------------------------------------------------------------------------
		static uint8_t* load_binary_file_data(stringview_t file_path, unsigned* data_size_out)
		{
			uint8_t* data = nullptr;
			*data_size_out = 0;

			if (file_path.data() != nullptr)
			{
				FILE* file = fopen(file_path.data(), "rb");

				if(file != NULL)
				{
					fseek(file, 0, SEEK_END);
					auto size = ftell(file);
					fseek(file, 0, SEEK_SET);

					if (size > 0)
					{
						data = (uint8_t*)CORE_MALLOC(sizeof(uint8_t) * size + 1);
						data[sizeof(uint8_t) * size - 1] = '\0';

						unsigned count = SCAST(unsigned, fread(data, sizeof(uint8_t), size, file));
						*data_size_out = count + 1;
					}

					fclose(file);
				}
				else
				{
					if (serror_reporter::instance().m_callback)
					{
						serror_reporter::instance().m_callback(logging_verbosity_error,
							fmt::format("Could not open file '{}'", file_path));
					}
				}
			}
			else
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_error,
						"Invalid file path provided for load_binary_file_data");
				}
			}
			return data;
		}

		//- @reference: raylib SaveFileData
		//------------------------------------------------------------------------------------------------------------------------
		static bool save_binary_file_data(stringview_t file_path, uint8_t* data, unsigned data_size)
		{
			if (file_path.data() != nullptr)
			{
				FILE* file = fopen(file_path.data(), "wb");

				if (file != NULL)
				{
					unsigned count = (unsigned)fwrite(data, sizeof(unsigned char), data_size, file);

					fclose(file);

					return count == data_size;
				}
				else
				{
					if (serror_reporter::instance().m_callback)
					{
						serror_reporter::instance().m_callback(logging_verbosity_error,
							fmt::format("Could not open file '{}'", file_path));
					}
				}
			}
			else
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_error,
						"Invalid file path provided for save_binary_file_data");
				}
			}
			return false;
		}

		//- @reference: raylib UnloadFileText.
		//- unload data allocated by load_text_file_data
		//------------------------------------------------------------------------------------------------------------------------
		static void unload_file_text_data(char* text)
		{
			CORE_FREE(text);
		}

		//- @reference: raylib LoadFileText
		//------------------------------------------------------------------------------------------------------------------------
		static char* load_text_file_data(stringview_t file_path)
		{
			char* text = nullptr;

			if (file_path.data() != nullptr)
			{
				FILE* file = fopen(file_path.data(), "rt");

				if (file != NULL)
				{
					fseek(file, 0, SEEK_END);
					auto size = SCAST(unsigned, ftell(file));
					fseek(file, 0, SEEK_SET);

					if (size > 0)
					{
						text = SCAST(char*, CORE_MALLOC((size + 1) * sizeof(char)));

						if (text != nullptr)
						{
							auto count = SCAST(unsigned, fread(text, sizeof(char), size, file));

							if (count < size)
							{
								text = SCAST(char*, CORE_REALLOC(text, count + 1));
							}

							text[count] = '\0';
						}
					}
					else
					{
						if (serror_reporter::instance().m_callback)
						{
							serror_reporter::instance().m_callback(logging_verbosity_error,
								fmt::format("Could not read text from file '{}'", file_path));
						}
					}
					fclose(file);
				}
				else
				{
					if (serror_reporter::instance().m_callback)
					{
						serror_reporter::instance().m_callback(logging_verbosity_error,
							fmt::format("Could not open file '{}'", file_path));
					}
				}
			}
			else
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_error,
						"Invalid file path provided for load_text_file_data");
				}
			}
			return text;
		}

		//- @reference: raylib SaveFileText
		//------------------------------------------------------------------------------------------------------------------------
		static bool save_text_file_data(stringview_t file_path, stringview_t text)
		{
			if (file_path.data() != nullptr)
			{
				FILE* file = fopen(file_path.data(), "wt");

				if (file != NULL)
				{
					auto count = fprintf(file, "%s", text.data());

					fclose(file);

					if (count > 0)
					{
						return true;
					}
					else
					{
						if (serror_reporter::instance().m_callback)
						{
							serror_reporter::instance().m_callback(logging_verbosity_error,
								fmt::format("Could not write text to file '{}'", file_path));
						}
					}
				}
				else
				{
					if (serror_reporter::instance().m_callback)
					{
						serror_reporter::instance().m_callback(logging_verbosity_error,
							fmt::format("Could not open file '{}'", file_path));
					}
				}
			}
			else
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_error,
						"Invalid file path provided for save_text_file_data");
				}
			}
			return false;
		}

	} //- unnamed

	namespace io
	{
		bool write_variant(const rttr::variant& var, nlohmann::json& json);
		rttr::variant from_json_recursively(rttr::type expected, const simdjson::dom::element& json);
		void from_json_recursively(rttr::variant& object_out, rttr::type expected, const simdjson::dom::element& json);

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TNumeric>
		rttr::variant extract_numeric_type(TNumeric value, rttr::type required_type)
		{
			if(rttr::type::get<float>() == required_type) return static_cast<float>(value);
			else if(rttr::type::get<double>() == required_type) return static_cast<double>(value);
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
			else if(type.is_class() || type.is_wrapper())
			{
				return simdjson::dom::element_type::OBJECT;
			}
			else if(type.is_sequential_container() || type.is_associative_container())
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
			else if(auto var = type.create(); var.is_valid())
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

					if(it.at_key(C_MAP_KEY_NAME).get(key_elem) != simdjson::SUCCESS ||
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
			if (!value.empty())
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
			else
			{
				//- Not an error, but nice to have, report quietly
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_debug,
						"\tIgnoring extraction of empty String");
				}
			}
			return {};
		}

		//------------------------------------------------------------------------------------------------------------------------
		void extract_from_object(rttr::variant& object_out, const simdjson::dom::object& json)
		{
			auto type = object_out.get_type();

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_debug,
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
						serror_reporter::instance().m_callback(logging_verbosity_debug,
							fmt::format("\tDeserializing property '{}' with type '{}'", name, prop_type));
					}

					simdjson::dom::element element;

					if(json.at_key(name).get(element) != simdjson::SUCCESS)
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
				serror_reporter::instance().m_callback(logging_verbosity_debug,
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
					serror_reporter::instance().m_callback(logging_verbosity_debug,
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
		rttr::variant from_json_blob(rttr::type expected, const uint8_t* data, unsigned size)
		{
			simdjson::dom::parser parser;
			simdjson::dom::element element;

			if (parser.parse(data, SCAST(size_t, size)).get(element) != simdjson::SUCCESS)
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
		rttr::variant from_json_string(rttr::type expected, const string_t& json)
		{
			return from_json_blob(expected, (const uint8_t*)json.data(), json.length());
		}

		//------------------------------------------------------------------------------------------------------------------------
		void to_json_recursive(const rttr::instance& object, nlohmann::json& json)
		{
			auto inst = object.get_type().get_raw_type().is_wrapper() ? object.get_wrapped_instance() : object;

			//-- wrap into object with type information
			//-- Note: type information is outside of the object to make deserializing easiert
			json[C_OBJECT_TYPE_NAME] = object.get_type().get_raw_type().get_name().data();
			json = nlohmann::json::object();

			for(const auto prop : inst.get_derived_type().get_properties())
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
			else if(type.is_enumeration())
			{
				//- try serializing as string
				bool result = false;
				if(auto s = var.to_string(&result); result)
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
					serror_reporter::instance().m_callback(logging_verbosity_debug,
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
			for(const auto& var : view)
			{
				write_variant(var, json[i++]);
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void write_map(const rttr::variant_associative_view& view, nlohmann::json& json)
		{
			json = nlohmann::json::array();

			auto i = 0;
			if(!view.is_key_only_type())
			{
				for(const auto& pair: view)
				{
					write_variant(pair.first, json[i][C_MAP_KEY_NAME.data()]);
					write_variant(pair.first, json[i][C_MAP_VALUE_NAME.data()]);
					++i;
				}
			}
			else
			{
				for(const auto& var : view)
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

			if(t.is_wrapper())
			{
				//- extract wrapped type
				t = t.get_wrapped_type();
				v = v.extract_wrapped_value();
			}

			//- number/bool/string/enum
			if(write_fundamental_type(t, v, json))
			{
				return true;
			}
			//- vector/array
			else if(t.is_sequential_container())
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

	namespace string_utils
	{
		//------------------------------------------------------------------------------------------------------------------------
		void split(const string_t& string, char delimiter, stl::vector<string_t>& storage)
		{
			std::stringstream ss(string.c_str());
			string_t token;

			while (std::getline(ss, token, delimiter))
			{
				storage.emplace_back(token);
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void insert(string_t& string, const string_t& to_insert_one, size_t index)
		{
			string.insert(index, to_insert_one);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void push_front(string_t& string, const string_t& to_prepend_one)
		{
			string.insert(0, to_prepend_one);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void push_back(string_t& string, const string_t& to_append_one)
		{
			string.append(to_append_one);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void erase_substr(string_t& string, const string_t& string_to_erase)
		{
			auto index = string.find(string_to_erase);
			if (index != string_t::npos)
			{
				string.erase(index, string_to_erase.size());
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void erase_range(string_t& string, size_t begin, size_t end)
		{
			string.erase(string.begin() + begin, string.begin() + end);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void to_lower(string_t& string)
		{
			std::transform(string.begin(), string.end(), string.begin(), [](unsigned char c) {return std::tolower(c); });
		}

		//------------------------------------------------------------------------------------------------------------------------
		size_t length(const string_t& string)
		{
			return string.size();
		}

		//------------------------------------------------------------------------------------------------------------------------
		size_t find_substr(const string_t& string, const string_t& substring)
		{
			return string.find(substring);
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool does_substr_exist(const string_t& string, const string_t& substring)
		{
			return string.find(substring) != string_t::npos;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool compare(const string_t& first, const string_t& second)
		{
			return first == second;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ends_with(stringview_t string, stringview_t substr)
		{
			if (string.length() >= substr.length())
			{
				return string.compare(string.length() - substr.length(), substr.length(), substr.data()) == 0;
			}
			return false;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool starts_with(stringview_t string, stringview_t substr)
		{
			if (string.length() >= substr.length())
			{
				return string.compare(0, substr.length(), substr.data()) == 0;
			}
			return false;
		}

	} //- string_utils

	namespace detail
	{
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

	} //- detail

	const array_t<unsigned char, 16u> cuuid::C_INVALID_DATA = { 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f' };
	const core::cuuid cuuid::C_INVALID_UUID = { C_INVALID_DATA };

	//------------------------------------------------------------------------------------------------------------------------
	string_t sinfo::platform()
	{
#if CORE_PLATFORM_WINDOWS
		return "Windows";
#elif CORE_PLATFORM_XBOXONE
		return "Xbox One";
#elif CORE_PLATFORM_XBOXSERIES
		return "Xbox Series";
#elif CORE_PLATFORM_LINUX
		return "Linux";
#elif CORE_PLATFORM_ANDROID
		return "Android";
#elif CORE_PLATFORM_IOS
		return "iOS";
#elif CORE_PLATFORM_OSX
		return "macOS";
#elif CORE_PLATFORM_PS4
		return "PlayStation 4";
#elif CORE_PLATFORM_PS5
		return "PlayStation 5";
#elif CORE_PLATFORM_NX
		return "NX";
#endif
		return "<unknown>";
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t sinfo::architecture()
	{
#if CORE_ARCH_64BIT
		return "x64";
#elif CORE_ARCH_32BIT
		return "x32";
#endif
		return "<unknown>";
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t sinfo::compiler()
	{
#if CORE_COMPILER_MSVC
		return "msvc";
#elif CORE_COMPILER_GCC
		return "gcc";
#elif CORE_COMPILER_CLANG
		return "clang";
#endif
		return "<unknown>";
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t sinfo::configuration()
	{
#if DEBUG
		return "Debug";
#elif RELEASE
		return "Release";
#elif HYBRID
		return "Hybrid";
#endif
		return "<unknown>";
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid()
	{
		generate(std::random_device()());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(const string_t& uuid)
	{
		parse_string(uuid, m_data);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(size_t seed)
	{
		generate(seed);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(const cuuid& other)
	{
		copy_from(other);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(array_t<unsigned char, 16u> data)
	{
		std::memcpy(&m_data[0], &data[0], sizeof(unsigned char) * 16);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::generate(size_t seed)
	{
		//- setup random number generator
		static thread_local std::mt19937_64 random_engine;
		static thread_local bool seeded = false;
		if (!seeded)
		{
			random_engine.seed(seed);
			seeded = true;
		}
		std::uniform_int_distribution<std::mt19937_64::result_type> dist(MIN(size_t), MAX(size_t));

		//- compute
		unsigned i, j, rnd;
		for (i = 0; i < (16 / C_RANDOM_BYTES_COUNT); i++)
		{
			rnd = SCAST(unsigned, dist(random_engine));
			for (j = 0; j < C_RANDOM_BYTES_COUNT; j++)
			{
				m_data[i * C_RANDOM_BYTES_COUNT + j] = (0xff & rnd >> (8 * j));
			}
		}
		//- set the version to 4
		m_data[6] = (m_data[6] & 0x0f) | 0x40;
		//- set the variant to 1 (a)
		m_data[8] = (m_data[8] & 0x0f) | 0xa0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::parse_string(const string_t& uuid, array_t<unsigned char, 16u>& out)
	{
		unsigned i = 0, j = 0;
		while (i < 36 && j < 16)
		{
			if (uuid[i] != '-')
			{
				out[j++] = (hex2dec(uuid[i++]) << 4) | hex2dec(uuid[i]);
			}
			i++;
		}

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::write_string(const array_t<unsigned char, 16>& data, string_t& out) const
	{
		out.resize(36);
		unsigned i = 0, j = 0;
		while (j < 16)
		{
			if (j == 4 || j == 6 || j == 8 || j == 10)
			{
				out[i++] = '-';
			}
			out[i++] = C_HEX[(data[j] >> 4)];
			out[i++] = C_HEX[(0xf & data[j])];
			j++;
		}
		out[36] = '\0';
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cuuid::hex2dec(char c)
	{
		for (auto i = 0u; i < 16; ++i)
		{
			if (C_HEX[i] == c)
			{
				return i;
			}
		}
		return -1;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::copy_to(cuuid& other)
	{
		std::memcpy(&other.m_data[0], &m_data[0], 16);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::copy_from(const cuuid& other)
	{
		std::memcpy(&m_data[0], &other.m_data[0], 16);
	}

	//------------------------------------------------------------------------------------------------------------------------
	int cuuid::compare(const cuuid& other) const
	{
		return std::memcmp(&m_data[0], &other.m_data[0], 16);
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cuuid::generate_string() const
	{
		string_t s;
		write_string(m_data, s);
		return s;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crandom::seed(unsigned value/* = 0*/)
	{
		if (value == 0)
		{
			C_RANDOM_ENGINE.seed(std::random_device()());
		}
		else
		{
			C_RANDOM_ENGINE.seed(value);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	int crandom::random_deviation(int value, int negative, int positive)
	{
		return in_range_int(value - negative, value + positive);
	}

	//------------------------------------------------------------------------------------------------------------------------
	float crandom::random_float()
	{
		return SCAST(float, m_distribution(C_RANDOM_ENGINE));
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned crandom::random_int()
	{
		return m_distribution(C_RANDOM_ENGINE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	float crandom::alternate_one_float()
	{
		return (m_distribution(C_RANDOM_ENGINE) % 2 == 0) ? -1.0f : 1.0f;
	}

	//------------------------------------------------------------------------------------------------------------------------
	int crandom::alternate_one_int()
	{
		return (m_distribution(C_RANDOM_ENGINE) % 2 == 0) ? -1 : 1;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float crandom::normalized_float()
	{
		return SCAST(float, m_distribution(C_RANDOM_ENGINE)) / MAX(unsigned);
	}

	//------------------------------------------------------------------------------------------------------------------------
	float crandom::in_range_float(float minimum, float maximum)
	{
		return minimum + normalized_float() * maximum;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned crandom::in_range_int(unsigned minimum, unsigned maximum)
	{
		std::uniform_int_distribution<std::mt19937::result_type> dist(minimum, maximum);

		return dist(C_RANDOM_ENGINE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t crandom::normalized_vec2()
	{
		return { normalized_float(), normalized_float() };
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t crandom::in_range_vec2(float minimum, float maximum)
	{
		return { in_range_float(minimum, maximum), in_range_float(minimum, maximum) };
	}

#if CORE_PLATFORM_WINDOWS || CORE_PLATFORM_XBOXONE || CORE_PLATFORM_XBOXSERIES
	#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <errno.h>
	typedef CRITICAL_SECTION pthread_mutex_t;
	typedef unsigned pthread_mutexattr_t;
	//------------------------------------------------------------------------------------------------------------------------
	inline int pthread_mutex_lock(pthread_mutex_t* _mutex)
	{
		EnterCriticalSection(_mutex);
		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	inline int pthread_mutex_unlock(pthread_mutex_t* _mutex)
	{
		LeaveCriticalSection(_mutex);
		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	inline int pthread_mutex_trylock(pthread_mutex_t* _mutex)
	{
		return TryEnterCriticalSection(_mutex) ? 0 : EBUSY;
	}

	//------------------------------------------------------------------------------------------------------------------------
	inline int pthread_mutex_init(pthread_mutex_t* _mutex, pthread_mutexattr_t* /*_attr*/)
	{
		InitializeCriticalSection(_mutex);
		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	inline int pthread_mutex_destroy(pthread_mutex_t* _mutex)
	{
		DeleteCriticalSection(_mutex);
		return 0;
	}
#endif

	//------------------------------------------------------------------------------------------------------------------------
	cmutex::cmutex()
	{
		static_assert(sizeof(pthread_mutex_t) <= sizeof(m_internal), "Critical");

		pthread_mutexattr_t attr;

#if CORE_PLATFORM_WINDOWS || CORE_PLATFORM_XBOXONE || CORE_PLATFORM_XBOXSERIES
#else
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#endif
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_init(handle, &attr);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmutex::~cmutex()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_destroy(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmutex::lock()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_lock(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmutex::unlock()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_unlock(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	memory_ref_t cmemory::make_ref(byte_t* data, unsigned size)
	{
		return std::make_shared<cmemory>(data, size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	memory_ref_t cmemory::make_ref(unsigned size)
	{
		return std::make_shared<cmemory>(size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmemory::cmemory(byte_t* data, unsigned size)
	{
		//- Copy data, freeing incoming data is not our responsibility
		m_data.reserve(SCAST(size_t, size));
		std::memcpy(m_data.data(), data, SCAST(size_t, size));
	}

	//- Allocate a piece of memory with requested size
	//------------------------------------------------------------------------------------------------------------------------
	cmemory::cmemory(unsigned size)
	{
		CORE_ASSERT(size > 0, "Invalid operation. Either empty memory or invalid memory requested!");

		m_data.reserve(SCAST(size_t, size));
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmemory::~cmemory()
	{
		m_data.clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	scolor::scolor(common_color color) :
		m_r(0), m_g(0), m_b(0), m_a(0)
	{
		constexpr array_t<uint8_t, 4> C_COMMON_COLORS[] =
		{
			{134, 38, 51, 255},
			{239, 83, 80, 255},
			{255, 205, 210, 255},

			{245, 124, 0, 255},
			{255, 171, 64, 255},
			{255, 224, 178, 255},

			{251, 192, 45, 255},
			{255, 235, 59, 255},
			{255, 245, 157, 255},

			{52, 168, 83, 255},
			{129, 199, 132, 255},
			{200, 230, 201, 255},

			{0, 188, 212, 255},
			{77, 208, 225, 255},
			{178, 235, 242, 255},

			{33, 150, 243, 255},
			{144, 202, 249, 255},
			{187, 222, 251, 255},

			{156, 39, 176, 255},
			{233, 30, 99, 255},
			{248, 187, 208, 255},

			{233, 30, 99, 255},
			{240, 98, 146, 255},
			{248, 187, 208, 255},

			{255, 255, 255, 255},
			{0, 0, 0, 255},
			{33, 33, 33, 255},
			{66, 66, 66, 255},
			{117, 117, 117, 255},
			{153, 153, 153, 255},
			{189, 189, 189, 255},
			{224, 224, 224, 255},
			{238, 238, 238, 255},
			{255, 255, 255, 255},
		};

		const auto& c = C_COMMON_COLORS[(unsigned)color];
		m_r = c[0];
		m_g = c[1];
		m_b = c[2];
		m_a = c[3];
	}

	//------------------------------------------------------------------------------------------------------------------------
	scolor::scolor(uint8_t r /*= 0*/, uint8_t g /*= 0*/, uint8_t b /*= 0*/, uint8_t a /*= 0*/) :
		m_r(r), m_g(g), m_b(b), m_a(a)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	scolor::scolor(vec4_t normalized) :
		m_r(normalized.r * 255),
		m_g(normalized.g * 255),
		m_b(normalized.b * 255),
		m_a(normalized.a * 255)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec4_t scolor::normalize() const
	{
		return { m_r / 255.0f, m_g / 255.0f, m_b / 255.0f, m_a / 255.0f };
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned scolor::rgba() const
	{
		return ((SCAST(unsigned, m_r) << 24) | (SCAST(unsigned, m_g) << 16) | (SCAST(unsigned, m_b) << 8) | SCAST(unsigned, m_a));
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned scolor::abgr() const
	{
		return ((SCAST(unsigned, m_a) << 24) | (SCAST(unsigned, m_b) << 16) | (SCAST(unsigned, m_g) << 8) | SCAST(unsigned, m_r));
	}

	//------------------------------------------------------------------------------------------------------------------------
	srect::srect(const vec2_t& xy, const vec2_t& wh) :
		m_x(xy.x), m_y(xy.y), m_w(wh.x), m_h(wh.y)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	srect::srect(float x /*= 0.0f*/, float y /*= 0.0f*/, float w /*= 0.0f*/, float h /*= 0.0f*/) :
		m_x(x), m_y(y), m_w(w), m_h(h)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void srect::set(float x, float y, float w, float h)
	{
		m_x = x;
		m_y = y;
		m_w = w;
		m_h = h;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void srect::position(float x, float y)
	{
		m_x = x;
		m_y = y;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void srect::dimension(float w, float h)
	{
		m_w = w;
		m_h = h;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cpath::update_strings()
	{
		m_string_path = m_path.generic_u8string().c_str();
		m_string_ext = m_path.extension().generic_u8string().c_str();
		m_string_stem = m_path.stem().generic_u8string().c_str();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath::cpath(const char* path) :
		m_path(path)
	{
		m_dir = std::filesystem::directory_entry(m_path);

		update_strings();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath::cpath(const std::filesystem::path& path) :
		m_path(path)
	{
		m_dir = std::filesystem::directory_entry(m_path);

		update_strings();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath::cpath(const cpath& path) :
		m_path(path.m_path)
	{
		m_dir = std::filesystem::directory_entry(m_path);

		update_strings();
	}

	//------------------------------------------------------------------------------------------------------------------------
	std::filesystem::path cpath::path() const
	{
		return m_path;
	}

	//------------------------------------------------------------------------------------------------------------------------
	std::filesystem::directory_entry cpath::dir() const
	{
		return m_dir;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::exists() const
	{
		return m_dir.exists();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::has_extension() const
	{
		return m_path.has_extension();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::has_parent() const
	{
		return m_path.has_parent_path();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::is_dir() const
	{
		return m_dir.is_directory();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::is_file() const
	{
		return m_dir.is_regular_file();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath cpath::parent() const
	{
		return { m_path.parent_path() };
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath& cpath::append(stringview_t path)
	{
		m_path = std::filesystem::path(fmt::format("{}{}", m_path.generic_u8string(), path).c_str());
		m_dir = std::filesystem::directory_entry(m_path);

		update_strings();

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::operator==(const std::filesystem::path& path)
	{
		return m_path == path;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cpath::operator==(stringview_t path)
	{
		return view() == path;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath& cpath::operator/=(stringview_t path)
	{
		m_path /= path;
		m_dir = std::filesystem::directory_entry(m_path);

		update_strings();

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath& cpath::operator=(const cpath& path)
	{
		m_path = path.m_path;
		m_dir = path.m_dir;

		update_strings();

		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cfilesystem::cfilesystem(cpath path) :
		m_current(std::move(path))
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cfilesystem::cfilesystem(const char* path) :
		m_current(path)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cpath cfilesystem::construct(const char* path, const char* addition)
	{
		return cpath(path).append(addition);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath cfilesystem::join(const char* path, const char* addition)
	{
		return cpath(path) /= addition;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cpath cfilesystem::cwd()
	{
		try
		{
			return std::filesystem::current_path().c_str();
		}
		catch (...)
		{
		}
		return cpath(C_EMPTY_STRING);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_dir(const char* path)
	{
		return std::filesystem::create_directory(path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_dir_in(const char* path, const char* name)
	{
		return create_dir(cpath(path).append(name).view());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_dir_recursive(const char* path)
	{
		return std::filesystem::create_directories(path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_file(const char* path)
	{
		std::ofstream out(path);

		std::filesystem::permissions(path,
			std::filesystem::perms::owner_all | std::filesystem::perms::group_all,
			std::filesystem::perm_options::add);

		out.close();

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_file_in(const char* path, const char* stem, const char* ext)
	{
		return create_file(fmt::format("{}/{}.{}", path, stem, ext).data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::remove(const char* path, const char* name)
	{
		cpath p(path);
		p.append(name);

		return remove(p.view());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::remove(const char* path)
	{
		//- filesystem throws on errors
		try
		{
			std::error_code err;

			return std::filesystem::remove_all(path, err) >= 1;
		}
		catch (const std::exception& /*e*/)
		{
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_file(const char* path, const char* name)
	{
		return find_at(path, name, filesystem_lookup_type_file);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_file_by_stem(const char* path, const char* name)
	{
		cpath p(path);
		if (p.exists())
		{
			for (const auto& entry : std::filesystem::directory_iterator{ p.path() })
			{
				if (entry.is_regular_file() && entry.path().stem().string().compare(name) == 0)
				{
					return true;
				}
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_file_by_extension(const char* path, const char* name)
	{
		cpath p(path);
		if (p.exists())
		{
			for (const auto& entry : std::filesystem::directory_iterator{ p.path() })
			{
				if (entry.is_regular_file() &&
					entry.path().has_extension() &&
					entry.path().extension().string().compare(name) == 0)
				{
					return true;
				}
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_dir(const char* path, const char* name)
	{
		return find_at(path, name, filesystem_lookup_type_directory);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_at(const char* path, const char* name, filesystem_lookup_type type)
	{
		cpath p(path);
		if (p.exists())
		{
			p.append(name);

			for (const auto& entry : std::filesystem::directory_iterator{ p.path() })
			{
				switch (type)
				{
				default:
				case filesystem_lookup_type_any:
				{
					if (p == entry.path())
					{
						return true;
					}
					break;
				}
				case filesystem_lookup_type_directory:
				{
					if (entry.is_directory() && p == entry.path())
					{
						return true;
					}
					break;
				}
				case filesystem_lookup_type_file:
				{
					if (entry.is_regular_file() && p == entry.path())
					{
						return true;
					}
					break;
				}
				}
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cpath cfilesystem::construct_relative_to_cwd(const char* path)
	{
		auto absolute = std::filesystem::absolute(cfilesystem::cwd().path());

		return std::filesystem::relative(path, absolute);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::is_contained(const cpath& contained, const cpath& container)
	{
		return container.path() / std::filesystem::relative(contained.path()) == contained.path();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::forwards(const char* path, bool forced /*= false*/)
	{
		//- create copy
		cpath copy(m_current);

		copy /= path;

		if (copy.exists())
		{
			m_current = std::move(copy);
			return true;
		}
		else if(forced)
		{
			return std::filesystem::create_directory(m_current.path());
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::backwards()
	{
		if (m_current.has_parent())
		{
			m_current = m_current.parent();
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cfilesystem::append(const char* path)
	{
		m_current /= path;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfilesystem& cfilesystem::operator/=(const char* path)
	{
		append(path);
		return *this;
	}

	using namespace std::chrono_literals;

	//------------------------------------------------------------------------------------------------------------------------
	ctimer::ctimer(bool paused /*= true*/) :
		m_timepoint(0s)
	{
		if (!paused)
		{
			start();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctimer::start()
	{
		m_timepoint = std::chrono::high_resolution_clock::now();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool ctimer::started() const
	{
		return m_timepoint.time_since_epoch().count() > 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float ctimer::secs() const
	{
		static constexpr float C_TO_SEC = 1000.0f * 1000.0f;

		return microsecs() / C_TO_SEC;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float ctimer::millisecs() const
	{
		static constexpr float C_TO_MILLISEC = 1000.0f;

		return microsecs() / C_TO_MILLISEC;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float ctimer::microsecs() const
	{
		ASSERT(started(), "Invalid operation. Timer must be started before it can be used");

		auto now = std::chrono::high_resolution_clock::now();

		return SCAST(float, std::chrono::time_point_cast<std::chrono::microseconds>(now).time_since_epoch().count() -
			std::chrono::time_point_cast<std::chrono::microseconds>(m_timepoint).time_since_epoch().count());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cany::~cany()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cany& cany::operator=(const cany& other)
	{
		m_data = other.m_data;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cscope_mutex::cscope_mutex(cmutex& m) :
		m_mutex(m)
	{
		m_mutex.lock();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cscope_mutex::~cscope_mutex()
	{
		m_mutex.unlock();
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cservice* cservice_manager::get_base_service(rttr::variant& var)
	{
		return var.get_value<core::cservice*>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::init()
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto* service = get_base_service(s_services[i]);

			if (!service->on_start())
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_error,
						fmt::format("Failed starting service '{}'", s_services[i].get_type().get_name().data()));
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::shutdown()
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto* service = get_base_service(s_services[i]);

			service->on_shutdown();

			//- delete underlying raw pointers
			auto type = s_services[i].get_type();

			if (type.is_pointer())
			{
				type.destroy(s_services[i]);
			}

			s_services[i].clear();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cservice_manager::on_update(float dt)
	{
		for (auto i = 0; i < s_service_count; ++i)
		{
			auto* service = get_base_service(s_services[i]);

			service->on_update(dt);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cservice* cservice_manager::find(const rttr::type& type)
	{
		auto id = type.get_id();

		if (s_service_types.find(id) != s_service_types.end())
		{
			auto& var = s_services[s_service_types[id]];

			ASSERT(var.is_valid(), "Invalid operation. Trying to retrieve a released service");

			return get_base_service(var);
		}

		ASSERT(false, "Invalid operation. Service does not exist");

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cservice_manager::emplace(rttr::type service_type)
	{
		if (s_next_type < core::cservice::C_SERVICE_COUNT_MAX && service_type.is_valid())
		{
			auto var = service_type.create({});

			if (var.is_valid())
			{
				auto id = service_type.get_id();
				auto t = s_next_type++;
				s_service_types[id] = t;
				s_services[t] = std::move(var);
				s_service_count++;
				return true;
			}
			else
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_error,
						fmt::format("\tFailed constructing service '{}'. Did you register a default constructor?",
							service_type.get_name().data()));
				}
			}
		}
		else
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_error,
					fmt::format("\tCan not emplace service '{}'. Maximum number of services reached, increase value in 'core' library", service_type.get_name().data()));
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cevent_service::~cevent_service()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cevent_service::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cevent_service::on_shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cevent_service::on_update(float)
	{
		core::cscope_mutex m(m_mutex);

		for (auto& pair : m_queue)
		{
			//- An event might note have any listeners, so we check here
			if (auto it = m_listeners.find(pair.first); it != m_listeners.end())
			{
				const auto& listeners = it->second;
				auto& queue = pair.second;

				while (!queue.empty())
				{
					const auto& e = queue.front();

					//- delegate the event to all intersted listeners and dispose of it after
					for (const auto& l : listeners)
					{
						l(e);
					}

					queue.pop();
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	iallocator::~iallocator()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	uint64_t iallocator::capacity() const
	{
		return m_total_size;
	}

	//------------------------------------------------------------------------------------------------------------------------
	uint64_t iallocator::used() const
	{
		return m_used_size;
	}

	//------------------------------------------------------------------------------------------------------------------------
	uint64_t iallocator::peak_used() const
	{
		return m_peak_used_size;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void iallocator::init(uint64_t size)
	{
		m_total_size = size;
	}

	//- Note: this tracks a memory allocation or deallocation depending on given allocation size.
	//------------------------------------------------------------------------------------------------------------------------
	void iallocator::track_allocation(int64_t allocation_size, uint64_t padding /*= 0*/)
	{
		m_used_size += (allocation_size + padding);
		m_peak_used_size = glm::max(m_peak_used_size, m_used_size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	clinear_allocator::clinear_allocator(uint64_t size) :
		m_offset(0), m_memory(nullptr)
	{
		//- real allocation
		m_memory = CORE_MALLOC(size);

		init(size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	clinear_allocator::~clinear_allocator()
	{
		//- real deallocation
		CORE_FREE(m_memory);

		m_memory = nullptr;
		m_offset = 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void* clinear_allocator::allocate(uint64_t size, uint64_t alignment /*= iallocator::C_ALIGNMENT*/)
	{
		CORE_ASSERT(detail::is_power_of_2(alignment), "Invalid operation. Alignment must be 0 or a power of 2!");

		uint64_t padding = 0;

		const auto current = RCAST(uint64_t, m_memory) + m_offset;

		//- check if we need to align memory
		if (alignment > 0 && m_offset % alignment != 0)
		{
			padding = detail::align_address_forward(current, alignment);
		}

		if ((m_offset + padding + size) < capacity())
		{
			m_offset = (padding + size);
			const auto result = current + padding;

			track_allocation(size, padding);

			return RCAST(void*, result);
		}

		//- No memory left for use
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void clinear_allocator::deallocate(void* ptr)
	{
		//- noop
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstack_allocator::cstack_allocator(uint64_t size) :
		m_offset(0), m_memory(nullptr)
	{
		//- real allocation
		m_memory = CORE_MALLOC(size);

		init(size);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cstack_allocator::~cstack_allocator()
	{
		//- real deallocation
		CORE_ASSERT(m_used_size == 0, "Invalid operation. Objects were not deallocated and are still in use!");

		CORE_FREE(m_memory);

		m_memory = nullptr;
		m_offset = 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void* cstack_allocator::allocate(uint64_t size, uint64_t alignment /*= iallocator::C_ALIGNMENT*/)
	{
		CORE_ASSERT(detail::is_power_of_2(alignment), "Invalid operation. Alignment must be 0 or a power of 2!");

		uint64_t padding = 0;

		const auto current = RCAST(uint64_t, m_memory) + m_offset;

		padding = detail::align_address_forward_with_header<sheader>(size, alignment);

		if ((m_offset + padding + size) < capacity())
		{
			m_offset += (padding + size);
			const auto result = current + padding;
			const auto header_address = result - sizeof(sheader);

			sheader* header_pointer = RCAST(sheader*, header_address);
			header_pointer->m_padding = padding;
			header_pointer->m_size = size;

			track_allocation(size, padding);

			return RCAST(void*, result);
		}

		//- No memory left for use
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cstack_allocator::deallocate(void* ptr)
	{
		const auto current = RCAST(uint64_t, ptr);
		const auto header_address = current - sizeof(sheader);
		const sheader* header_pointer{ RCAST(sheader*, header_address) };
		const auto padding = SCAST(uint64_t, header_pointer->m_padding);

		m_offset = current - padding - RCAST(uint64_t, m_memory);

		//- alignment padding is already included inside header pointer padding
		track_allocation(-(padding + header_pointer->m_size), 0);
	}

	namespace fs
	{
		//------------------------------------------------------------------------------------------------------------------------
		cfileinfo::cfileinfo(stringview_t filepath) :
			std::filesystem::path(filepath.data()), m_relative(filepath.data())
		{
			init();
		}

		//------------------------------------------------------------------------------------------------------------------------
		cfileinfo::cfileinfo(const cfileinfo& other) :
			std::filesystem::path(other.generic_u8string()), m_relative(other.m_relative), m_directory(other.m_directory), m_exists(other.m_exists), m_size(other.m_size)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		cfileinfo::cfileinfo(stringview_t basepath, stringview_t filepath) :
			std::filesystem::path(fmt::format("{}{}", basepath.data(), filepath.data()))
		{
			if (core::string_utils::starts_with(filepath, "/"))
			{
				//- ignore the '/'
				m_relative = filepath.substr(1).data();
			}
			else
			{
				m_relative = filepath.data();
			}

			init();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cfileinfo::init()
		{
			const auto entry = std::filesystem::directory_entry(*this);

			m_directory = entry.is_directory();
			m_exists = entry.exists();

			//- std::filesystem throws on errors
			try
			{
				m_size = SCAST(unsigned, entry.file_size());
			}
			catch (...)
			{
			}
		}

		//- Path is the path concatenated from a filesystems basepath and the filename
		//------------------------------------------------------------------------------------------------------------------------
		string_t cfileinfo::path() const
		{
			return base_t::generic_u8string();
		}

		//- Relative path is the path without the filesystems basepath
		//------------------------------------------------------------------------------------------------------------------------
		string_t cfileinfo::relative() const
		{
			return m_relative;
		}

		//------------------------------------------------------------------------------------------------------------------------
		string_t cfileinfo::name() const
		{
			return base_t::filename().generic_u8string();
		}

		//------------------------------------------------------------------------------------------------------------------------
		string_t cfileinfo::stem() const
		{
			return base_t::stem().generic_u8string();
		}

		//------------------------------------------------------------------------------------------------------------------------
		string_t cfileinfo::ext() const
		{
			return base_t::extension().generic_u8string();
		}

		//------------------------------------------------------------------------------------------------------------------------
		string_t cfileinfo::directory_path() const
		{
			return base_t::parent_path().generic_u8string();
		}

		//------------------------------------------------------------------------------------------------------------------------
		unsigned cfileinfo::size() const
		{
			return m_size;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cfileinfo::is_file() const
		{
			return !is_directory();
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cfileinfo::operator==(const cfileinfo& other) const
		{
			return path() == other.path();
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cfileinfo::is_directory() const
		{
			return m_directory;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cvirtual_filesystem::cvirtual_filesystem()
		{
			m_filesystems.reserve(C_FILESYSTEMS_RESERVE_COUNT);
			m_opened_files.reserve(C_FILESYSTEM_OPENED_FILES_RESERVE_COUNT);

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_debug, "Created Virtual File System");
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		cvirtual_filesystem::~cvirtual_filesystem()
		{
			std::for_each(m_filesystems.begin(), m_filesystems.end(), [](const auto& pair)
				{
					pair.second->shutdown();
				});
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cvirtual_filesystem::on_start()
		{
			return true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cvirtual_filesystem::on_shutdown()
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cvirtual_filesystem::on_update(float)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cvirtual_filesystem::add_filesystem(stringview_t alias, filesystem_ref_t filesystem)
		{
			if (filesystem)
			{
				string_t _alias = alias.data();

				if (!core::string_utils::ends_with(_alias, "/"))
				{
					_alias += "/";
				}

				if (const auto it = m_filesystems.find(_alias); it == m_filesystems.end())
				{
					m_filesystems[_alias] = filesystem;
					m_sorted_aliases[_alias] = filesystem;

					if (serror_reporter::instance().m_callback)
					{
						serror_reporter::instance().m_callback(logging_verbosity_info,
							fmt::format("Filesystem alias '{}' added to Virtual File System", _alias));
					}
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cvirtual_filesystem::remove_filesystem(stringview_t alias)
		{
			string_t _alias = alias.data();

			if (!core::string_utils::ends_with(_alias, "/"))
			{
				_alias += "/";
			}

			if (const auto it = m_filesystems.find(_alias); it != m_filesystems.end())
			{
				m_filesystems.erase(it);
			}

			if (const auto it = m_sorted_aliases.find(_alias); it != m_sorted_aliases.end())
			{
				m_sorted_aliases.erase(it);
			}

			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(logging_verbosity_info,
					fmt::format("Filesystem alias '{}' removed from Virtual File System", _alias));
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cvirtual_filesystem::does_filesystem_exists(stringview_t alias) const
		{
			return m_filesystems.find(alias.data()) != m_filesystems.end();
		}

		//------------------------------------------------------------------------------------------------------------------------
		core::fs::filesystem_ref_t cvirtual_filesystem::find_filesystem(stringview_t alias) const
		{
			return m_filesystems.at(alias.data());
		}

		//------------------------------------------------------------------------------------------------------------------------
		core::fs::file_ref_t cvirtual_filesystem::open(const cfileinfo& filepath, int file_mode)
		{
			file_ref_t file = nullptr;

			if (std::all_of(m_sorted_aliases.begin(), m_sorted_aliases.end(), [&](const auto& pair)
				{
					const auto& path = pair.first;
					const auto& filesystem = pair.second;

					if (core::string_utils::starts_with(filepath.path(), path) &&
						filepath.path().length() != path.length())
					{
						file = filesystem->open(filepath, file_mode);
					}

					if (file)
					{
						uint64_t address = RCAST(uint64_t, (void*)file.get());
						m_opened_files[address] = filesystem;

						//- Opened file. Stop.
						return false;
					}
					//- Did not find file. Proceed.
					return true;
				}))
			{
				//- report success or failure
				if (serror_reporter::instance().m_callback)
				{
					if (file)
					{
						serror_reporter::instance().m_callback(logging_verbosity_info,
							fmt::format("Successfully opening file '{}' with mode '{}'", filepath.name(), file_mode));
					}
					else
					{
						serror_reporter::instance().m_callback(logging_verbosity_warn,
							fmt::format("Failed opening file '{}' with mode '{}'", filepath.name(), file_mode));
					}
				}
			}

			return file;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cvirtual_filesystem::close(file_ref_t file)
		{
			uint64_t address = RCAST(uint64_t, (void*)file.get());

			if (const auto it = m_opened_files.find(address); it != m_opened_files.end())
			{
				//- Close file and erase from storage
				it->second->close(file);

				m_opened_files.erase(it);

				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_info,
						fmt::format("Successfully closed file '{}'", file->info().name()));
				}
			}
			else
			{
				if (serror_reporter::instance().m_callback)
				{
					serror_reporter::instance().m_callback(logging_verbosity_warn,
						fmt::format("Failed closing file '{}'", file->info().name()));
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		vector_t<core::fs::cfileinfo> cvirtual_filesystem::iterate(const cfileinfo& filepath,
			filesystem_lookup_type type, bool recursive) const
		{
			vector_t<core::fs::cfileinfo> result;

			if (std::all_of(m_sorted_aliases.begin(), m_sorted_aliases.end(), [&](const auto& pair)
				{
					const auto& path = pair.first;
					const auto& filesystem = pair.second;

					if (core::string_utils::starts_with(filepath.path(), path) &&
						filepath.path().length() != path.length())
					{
						const auto files = filesystem->iterate(filepath, type, recursive);

						algorithm::insert(result, files.begin(), files.end());

						return false;
					}

					return true;
				}))
			{
			}

			return result;
		}

		//------------------------------------------------------------------------------------------------------------------------
		memory_ref_t load_text_from_file(stringview_t filepath)
		{
			memory_ref_t result = nullptr;

			if (const auto* text = load_text_file_data(filepath.data()); text)
			{
				//- copy loaded data
				result = cmemory::make_ref((byte_t*)text, (unsigned)strlen(text));

				//- free original data
				unload_file_text_data((char*)text);
			}

			return result;
		}

		//------------------------------------------------------------------------------------------------------------------------
		core::cfuture_type<memory_ref_t> load_text_from_file_async(stringview_t filepath)
		{
			auto result = casync::launch_async([p = filepath]()
				{
					return load_text_from_file(p);

				}).share();

			return result;
		}

		//------------------------------------------------------------------------------------------------------------------------
		memory_ref_t load_binary_from_file(stringview_t filepath)
		{
			memory_ref_t result = nullptr;
			unsigned size = 0;

			if (const auto* data = load_binary_file_data(filepath.data(), &size); data && size > 0)
			{
				//- copy loaded data
				result = cmemory::make_ref((byte_t*)data, size);

				//- free original data
				unload_file_binary_data((void*)data);
			}

			return result;
		}

		//------------------------------------------------------------------------------------------------------------------------
		core::cfuture_type<memory_ref_t> load_binary_from_file_async(stringview_t filepath)
		{
			auto result = casync::launch_async([p = filepath]()
				{
					return load_binary_from_file(p);

				}).share();

			return result;
		}

		//------------------------------------------------------------------------------------------------------------------------
		core::file_io_status save_text_to_file(stringview_t filepath, const char* string)
		{
			return save_text_file_data(filepath, string) ? file_io_status_success : file_io_status_failed;
		}

		//------------------------------------------------------------------------------------------------------------------------
		core::file_io_status save_text_to_file(stringview_t filepath, const memory_ref_t& data)
		{
			return save_text_to_file(filepath, (const char*)data->data());
		}

		//------------------------------------------------------------------------------------------------------------------------
		core::cfuture_type<core::file_io_status> save_text_to_file_async(stringview_t filepath, const memory_ref_t& data)
		{
			auto result = casync::launch_async([p = filepath, d = data]()
				{
					return save_text_to_file(p, d);

				}).share();

			return result;
		}

		//------------------------------------------------------------------------------------------------------------------------
		core::cfuture_type<core::file_io_status> save_text_to_file_async(stringview_t filepath, const char* string)
		{
			auto result = casync::launch_async([p = filepath, s = string]()
				{
					return save_text_to_file(p, s);

				}).share();

				return result;
		}

		//------------------------------------------------------------------------------------------------------------------------
		core::file_io_status save_binary_to_file(stringview_t filepath, const memory_ref_t& data)
		{
			return save_binary_to_file(filepath, (void*)data->data(), (unsigned)data->size());
		}

		//------------------------------------------------------------------------------------------------------------------------
		core::file_io_status save_binary_to_file(stringview_t filepath, void* data, unsigned size)
		{
			return save_binary_file_data(filepath, (uint8_t*)data, size) ? file_io_status_success : file_io_status_failed;
		}

		//------------------------------------------------------------------------------------------------------------------------
		core::cfuture_type<core::file_io_status> save_binary_to_file_async(stringview_t filepath, const memory_ref_t& data)
		{
			auto result = casync::launch_async([p = filepath, d = data]()
				{
					return save_binary_to_file(p, d);

				}).share();

				return result;
		}

		//------------------------------------------------------------------------------------------------------------------------
		core::cfuture_type<core::file_io_status> save_binary_to_file_async(stringview_t filepath, void* data, unsigned size)
		{
			auto result = casync::launch_async([p = filepath, d = data, s = size]()
				{
					return save_binary_to_file(p, d, s);

				}).share();

				return result;
		}

	} //- fs

	//------------------------------------------------------------------------------------------------------------------------
	void set_logger(error_report_function_t callback)
	{
		serror_reporter::instance().m_callback = std::move(callback);
	}

	namespace profile
	{
		namespace memory
		{
			namespace
			{
				constexpr stringview_t C_TOTAL_RESERVED_MEMORY_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_PEAK_TOTAL_RESERVED_MEMORY_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_TOTAL_USED_MEMORY_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_PEAK_TOTAL_USED_MEMORY_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_ELAPSED_PROCESS_TIME_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_ELAPSED_USER_CODE_TIME_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_ELAPSED_SYSTEM_CODE_TIME_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_TOTAL_RESIDENT_SET_SIZE_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_PEAK_TOTAL_RESIDENT_SET_SIZE_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_NORMAL_ALLOCATIONS_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_BIG_ALLOCATIONS_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_HUGE_ALLOCATIONS_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_MODIFICATIONS_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_RESET_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_PURGE_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_SYSTEM_CALL_MAP_MEMORY_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_SYSTEM_CALL_COMMIT_MEMORY_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_FREE_BLOCK_SEARCH_OPS_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_SEGMENTS_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				constexpr stringview_t C_PAGES_COUNT_DESC = "Current memory reserved by the program and potentially unused";
				static core::cmutex S_MUTEX;
				static aggregator_ref_t S_AGGREGATOR = nullptr;

				//------------------------------------------------------------------------------------------------------------------------
				static mi_stats_t* mi_stats_get_default(void)
				{
					mi_heap_t* heap = mi_heap_get_default();
					return &heap->tld->stats;
				}

				//------------------------------------------------------------------------------------------------------------------------
				template<typename TValue>
				static smemory_stats_entry pair(TValue value, stringview_t desc)
				{
					return { static_cast<uint64_t>(value), desc };
				}

				//- Note: be careful when watching mimalloc stats, it seems that each thread has its own stats making the data
				//- not that reliable.
				//------------------------------------------------------------------------------------------------------------------------
				class cdefault_aggregator final : public iaggregator
				{
				public:
					cdefault_aggregator() = default;
					~cdefault_aggregator() = default;

					smemory_stats	stats() override final;
					void			update() override final;

				private:
					smemory_stats m_current;
				};

				//------------------------------------------------------------------------------------------------------------------------
				void cdefault_aggregator::update()
				{
					core::cscope_mutex m(S_MUTEX);

					//- Gather and update current data
					uint64_t elapsed;
					uint64_t user_time;
					uint64_t sys_time;
					uint64_t current_rss;
					uint64_t peak_rss;
					uint64_t current_commit;
					uint64_t peak_commit;
					uint64_t page_faults;
					mi_process_info(&elapsed, &user_time, &sys_time, &current_rss, &peak_rss, &current_commit, &peak_commit, &page_faults);

					const auto* mistats = mi_stats_get_default();

					m_current.m_total_reserved_memory = pair(mistats->reserved.current, C_TOTAL_RESERVED_MEMORY_DESC);
					m_current.m_peak_total_reserved_memory = pair(mistats->reserved.peak, C_PEAK_TOTAL_RESERVED_MEMORY_DESC);
					m_current.m_total_used_memory = pair(mistats->committed.current, C_TOTAL_USED_MEMORY_DESC);
					m_current.m_peak_total_used_memory = pair(mistats->committed.peak, C_PEAK_TOTAL_USED_MEMORY_DESC);

					m_current.m_elapsed_process_time = pair(elapsed, C_ELAPSED_PROCESS_TIME_DESC);
					m_current.m_elapsed_user_code_time = pair(user_time, C_ELAPSED_USER_CODE_TIME_DESC);
					m_current.m_elapsed_system_code_time = pair(sys_time, C_ELAPSED_SYSTEM_CODE_TIME_DESC);

					m_current.m_total_resident_set_size = pair(current_rss, C_TOTAL_RESIDENT_SET_SIZE_DESC);
					m_current.m_peak_total_resident_set_size = pair(peak_rss, C_PEAK_TOTAL_RESIDENT_SET_SIZE_DESC);

					m_current.m_normal_allocations_bytes = pair(mistats->normal.current, C_NORMAL_ALLOCATIONS_COUNT_DESC);
					m_current.m_large_allocations_bytes = pair(mistats->large.current, C_BIG_ALLOCATIONS_COUNT_DESC);
					m_current.m_huge_allocations_bytes = pair(mistats->huge.current, C_HUGE_ALLOCATIONS_COUNT_DESC);

					//- Note: for mi_stat_counter_t we only care for 'total' value, i.e. the accumulated bytes or operation counts,
					//- the frequency of those operations occuring is denoted with 'count'.
					m_current.m_modifications_count = pair(mistats->page_committed.current, C_MODIFICATIONS_COUNT_DESC);
					m_current.m_reset_count = pair(mistats->reset_calls.total, C_RESET_COUNT_DESC);
					m_current.m_purge_count = pair(mistats->purge_calls.total, C_PURGE_COUNT_DESC);
					m_current.m_system_call_memory_map_count = pair(mistats->mmap_calls.total, C_SYSTEM_CALL_MAP_MEMORY_DESC);
					m_current.m_system_call_memory_commit_count = pair(mistats->commit_calls.total, C_SYSTEM_CALL_COMMIT_MEMORY_DESC);
					m_current.m_free_block_search_operations_count = pair(mistats->searches.total, C_FREE_BLOCK_SEARCH_OPS_COUNT_DESC);

					m_current.m_segments_count = pair(mistats->segments.current, C_SEGMENTS_COUNT_DESC);
					m_current.m_pages_count = pair(mistats->pages.current, C_PAGES_COUNT_DESC);
				}

				//------------------------------------------------------------------------------------------------------------------------
				smemory_stats cdefault_aggregator::stats()
				{
					core::cscope_mutex m(S_MUTEX);

					return m_current;
				}

			} //- unnamed

			//------------------------------------------------------------------------------------------------------------------------
			void set_aggregator(aggregator_ref_t object)
			{
				S_AGGREGATOR = object;
			}

			//------------------------------------------------------------------------------------------------------------------------
			aggregator_ref_t get_aggregator()
			{
				return S_AGGREGATOR;
			}

			//------------------------------------------------------------------------------------------------------------------------
			void set_default_aggregator()
			{
#if MIMALLOC_ENABLE
				S_AGGREGATOR = std::make_shared<cdefault_aggregator>();

				//- Gather initial data
				S_AGGREGATOR->update();
#endif
			}

		} //- memory

		namespace cpu
		{
			namespace
			{
				static core::cmutex S_MUTEX;
				static aggregator_ref_t S_AGGREGATOR = nullptr;

				//- Class responsible for aggregating raw CPU and function profiling data.
				//------------------------------------------------------------------------------------------------------------------------
				class cdefault_aggregator final : public iaggregator
				{
				public:
					cdefault_aggregator() = default;
					~cdefault_aggregator() = default;

					vector_t<scpu_stats>	stats() override final;
					void					update() override final;
					void					push(sfunction_data&& data) override final;

					saggregated_function_data						function_data(stringview_t name) override final;
					umap_t<const char*, saggregated_function_data>	function_data() override final;

				private:
					umap_t<const char*, saggregated_function_data> m_functions;
					stack_t<sfunction_data> m_stack;
					vector_t<scpu_stats> m_current;

				private:
					void gather_hardware_information();
					void gather_function_information();
				};

				//------------------------------------------------------------------------------------------------------------------------
				vector_t<cpu::scpu_stats> cdefault_aggregator::stats()
				{
					core::cscope_mutex m(S_MUTEX);

					return m_current;
				}

				//------------------------------------------------------------------------------------------------------------------------
				void cdefault_aggregator::update()
				{
					core::cscope_mutex m(S_MUTEX);

					gather_hardware_information();
					gather_function_information();
				}

				//------------------------------------------------------------------------------------------------------------------------
				void cdefault_aggregator::push(sfunction_data&& data)
				{
					core::cscope_mutex m(S_MUTEX);

					m_stack.push(std::move(data));
				}

				//------------------------------------------------------------------------------------------------------------------------
				saggregated_function_data cdefault_aggregator::function_data(stringview_t name)
				{
					core::cscope_mutex m(S_MUTEX);

					return m_functions.at(name.data());
				}

				//------------------------------------------------------------------------------------------------------------------------
				umap_t<const char*, saggregated_function_data> cdefault_aggregator::function_data()
				{
					core::cscope_mutex m(S_MUTEX);

					return m_functions;
				}

				//------------------------------------------------------------------------------------------------------------------------
				void cdefault_aggregator::gather_hardware_information()
				{
					m_current.clear();

					for (const auto& info : hwinfo::getAllCPUs())
					{
						scpu_stats cpu_stat;

						cpu_stat.m_model_vendor				= fmt::format("{}-{}", info.vendor(), info.modelName());
						cpu_stat.m_cores_logical			= static_cast<uint8_t>(info.numLogicalCores());
						cpu_stat.m_cores_physical			= static_cast<uint8_t>(info.numPhysicalCores());
						cpu_stat.m_core_max_clock_speed		= info.maxClockSpeed_MHz();
						cpu_stat.m_core_clock_speed			= info.regularClockSpeed_MHz();
						cpu_stat.m_load						= static_cast<float>(info.currentUtilisation());
						cpu_stat.m_cache_size_L1			= info.L1CacheSize_Bytes();
						cpu_stat.m_cache_size_L2			= info.L2CacheSize_Bytes();
						cpu_stat.m_cache_size_L3			= info.L3CacheSize_Bytes();

						m_current.push_back(cpu_stat);
					}
				}

				//------------------------------------------------------------------------------------------------------------------------
				void cdefault_aggregator::gather_function_information()
				{
					while (!m_stack.empty())
					{
						const auto& fun = m_stack.top(); m_stack.pop();

						auto& data = m_functions[fun.m_name];

						data.m_data.m_thread = fun.m_thread;
						data.m_data.m_time = fun.m_time;
						data.m_data.m_name = fun.m_name;
						data.m_data.m_category = fun.m_category;
						data.m_data.m_file = fun.m_file;
						data.m_data.m_file_line = fun.m_file_line;

						++data.m_callcount;

						data.m_time_cumulative += fun.m_time;
						data.m_time_peak = glm::max(data.m_time_peak, fun.m_time);
						data.m_time_average = (data.m_time_cumulative / static_cast<float>(data.m_callcount));
					}
				}

			} //- unnamed

			//------------------------------------------------------------------------------------------------------------------------
			void set_aggregator(aggregator_ref_t object)
			{
				S_AGGREGATOR = object;
			}

			//------------------------------------------------------------------------------------------------------------------------
			void set_default_aggregator()
			{
				S_AGGREGATOR = std::make_shared<cdefault_aggregator>();
			}

			//------------------------------------------------------------------------------------------------------------------------
			aggregator_ref_t get_aggregator()
			{
				return S_AGGREGATOR;
			}

			//------------------------------------------------------------------------------------------------------------------------
			cscoped_function::cscoped_function(uint64_t thread, const char* name, const char* category, const char* file, unsigned line) :
				m_data({thread, 0.0f, name, category, file, line})
			{
				m_timer.start();
			}

			//------------------------------------------------------------------------------------------------------------------------
			cscoped_function::~cscoped_function()
			{
				m_data.m_time = m_timer.microsecs();

				core::profile::cprofiler::push(std::move(m_data));
			}

		} //- cpu

		namespace gpu
		{

		} //- gpu

		//------------------------------------------------------------------------------------------------------------------------
		void cprofiler::init()
		{
			cpu::set_default_aggregator();
			memory::set_default_aggregator();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cprofiler::push(cpu::sfunction_data&& data)
		{
			CORE_ASSERT(cpu::get_aggregator(), "Invalid operation. No CPU aggregator was set!");

			cpu::get_aggregator()->push(std::move(data));
		}

		//------------------------------------------------------------------------------------------------------------------------
		vector_t<cpu::scpu_stats> cprofiler::cpu_stats()
		{
			CORE_ASSERT(cpu::get_aggregator(), "Invalid operation. No CPU aggregator was set!");

			return cpu::get_aggregator()->stats();
		}

		//------------------------------------------------------------------------------------------------------------------------
		memory::smemory_stats cprofiler::memory_stats()
		{
			CORE_ASSERT(memory::get_aggregator(), "Invalid operation. No Memory aggregator was set!");

			return memory::get_aggregator()->stats();
		}

		//------------------------------------------------------------------------------------------------------------------------
		cpu::saggregated_function_data cprofiler::function_data(stringview_t name)
		{
			CORE_ASSERT(cpu::get_aggregator(), "Invalid operation. No CPU aggregator was set!");

			return cpu::get_aggregator()->function_data(name);
		}

		//------------------------------------------------------------------------------------------------------------------------
		umap_t<const char*, cpu::saggregated_function_data> cprofiler::function_data()
		{
			CORE_ASSERT(cpu::get_aggregator(), "Invalid operation. No CPU aggregator was set!");

			return cpu::get_aggregator()->function_data();
		}

	} //- profile

} //- core

namespace math
{
	//------------------------------------------------------------------------------------------------------------------------
	bool almost_equal(float a, float b, float e /*= C_ALMOST_EQUAL_EPSILON*/)
	{
		return glm::distance(glm::abs(a), glm::abs(b)) < C_ALMOST_EQUAL_EPSILON;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool almost_equal(const vec2_t& a, const vec2_t& b, float e /*= C_ALMOST_EQUAL_EPSILON*/)
	{
		return almost_equal(a.x, b.x, e) && almost_equal(a.y, b.y, e);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void transform(mat4_t& out, const vec2_t& position, const vec2_t& scale, const vec2_t& shear,
		float rotation, const mat4_t& parent /*= C_MAT4_ID*/)
	{
		static thread_local vec3_t S_POSITION = vec3_t(0.0f, 0.0f, 0.0f), S_DIMENSION = vec3_t(0.0f, 0.0f, 0.0f), S_Z_AXIS = vec3_t(0.0f, 0.0f, 1.0f);
		static thread_local float S_ROTATION = 0.0f;

		S_POSITION.x = position.x;
		S_POSITION.y = position.y;
		S_DIMENSION.x = scale.x;
		S_DIMENSION.y = scale.y;
		S_ROTATION = rotation;

		out = glm::translate(C_MAT4_ID, S_POSITION)
			* glm::rotate(C_MAT4_ID, S_ROTATION, S_Z_AXIS)
			* glm::scale(C_MAT4_ID, S_DIMENSION);
	}

	//------------------------------------------------------------------------------------------------------------------------
	std::tuple<vec2_t, vec2_t, float> transform(const vec2_t& position, const vec2_t& scale, const vec2_t& shear,
		float rotation, const mat4_t& parent /*= C_MAT4_ID*/)
	{
		CORE_NAMED_ZONE(math::transform);

		static thread_local vec3_t S_EULER_ANGLES = vec3_t(0.0f);
		static thread_local mat4_t S_TRANSFORM = mat4_t(1.0f);

		transform(S_TRANSFORM, position, scale, shear, rotation, parent);

		vec2_t p, s;
		float r;

		//- extract 2D position
		p.x = S_TRANSFORM[3].x;
		p.y = S_TRANSFORM[3].y;

		//- extract 2D scale
		s.x = glm::length(S_TRANSFORM[0]);
		s.y = glm::length(S_TRANSFORM[1]);

		//- extract rotation around Z axis
		glm::extractEulerAngleXYZ(S_TRANSFORM, S_EULER_ANGLES.x, S_EULER_ANGLES.y, S_EULER_ANGLES.z);
		r = S_EULER_ANGLES.z;

		return { p, s, glm::degrees(r) };
	}

} //- math