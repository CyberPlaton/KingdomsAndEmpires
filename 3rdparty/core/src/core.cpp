#include "core.hpp"
#include <sstream>

namespace algorithm
{
	//------------------------------------------------------------------------------------------------------------------------
	vec3_t decompose_rotation(const mat4_t& transform)
	{
		vec3_t out;

		decompose_rotation(transform, out);

		return out;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void decompose_rotation(const mat4_t& transform, vec3_t& out)
	{
		glm::extractEulerAngleXYZ(transform, out.x, out.y, out.z);
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec3_t decompose_translation(const mat4_t& transform)
	{
		return { transform[3] };
	}

	//------------------------------------------------------------------------------------------------------------------------
	void decompose_translation(const mat4_t& transform, vec3_t& out)
	{
		out = { transform[3] };
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec3_t decompose_scale(const mat4_t& transform)
	{
		return { glm::length(vec3_t(transform[0])), glm::length(vec3_t(transform[1])), glm::length(vec3_t(transform[2])) };
	}

	//------------------------------------------------------------------------------------------------------------------------
	void decompose_scale(const mat4_t& transform, vec3_t& out)
	{
		out.x = glm::length(vec3_t(transform[0]));
		out.y = glm::length(vec3_t(transform[1]));
		out.z = glm::length(vec3_t(transform[2]));
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned hash(stringview_t string)
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
	handle_type_t invalid_handle()
	{
		return MAX(handle_type_t);
	}

} //- algorithm

namespace io
{
	namespace
	{
		void fromjson_recursively(rttr::instance object, rapidjson::Value& json_object);

		//------------------------------------------------------------------------------------------------------------------------
		rttr::variant extract_basic_types(rapidjson::Value& json_value)
		{
			switch (json_value.GetType())
			{
			case rapidjson::kStringType:
			{
				return std::string(json_value.GetString());
				break;
			}
			case rapidjson::kNullType:     break;
			case rapidjson::kFalseType:
			case rapidjson::kTrueType:
			{
				return json_value.GetBool();
				break;
			}
			case rapidjson::kNumberType:
			{
				if (json_value.IsInt())
					return json_value.GetInt();
				else if (json_value.IsDouble())
					return json_value.GetDouble();
				else if (json_value.IsUint())
					return json_value.GetUint();
				else if (json_value.IsInt64())
					return json_value.GetInt64();
				else if (json_value.IsUint64())
					return json_value.GetUint64();
				break;
			}
			// we handle only the basic types here
			case rapidjson::kObjectType:
			case rapidjson::kArrayType: return rttr::variant();
			}

			return rttr::variant();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void write_array_recursively(rttr::variant_sequential_view& view, rapidjson::Value& json_array_value)
		{
			view.set_size(json_array_value.Size());
			const rttr::type array_value_type = view.get_rank_type(1);

			for (rapidjson::SizeType i = 0; i < json_array_value.Size(); ++i)
			{
				auto& json_index_value = json_array_value[i];
				if (json_index_value.IsArray())
				{
					auto sub_array_view = view.get_value(i).create_sequential_view();
					write_array_recursively(sub_array_view, json_index_value);
				}
				else if (json_index_value.IsObject())
				{
					rttr::variant var_tmp = view.get_value(i);
					rttr::variant wrapped_var = var_tmp.extract_wrapped_value();
					fromjson_recursively(wrapped_var, json_index_value);
					view.set_value(i, wrapped_var);
				}
				else
				{
					rttr::variant extracted_value = extract_basic_types(json_index_value);
					if (extracted_value.convert(array_value_type))
						view.set_value(i, extracted_value);
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		rttr::variant extract_value(rapidjson::Value::MemberIterator& itr, const rttr::type& t)
		{
			auto& json_value = itr->value;
			rttr::variant extracted_value = extract_basic_types(json_value);
			const bool could_convert = extracted_value.convert(t);
			if (!could_convert)
			{
				if (json_value.IsObject())
				{
					rttr::constructor ctor = t.get_constructor();
					for (auto& item : t.get_constructors())
					{
						if (item.get_instantiated_type() == t)
							ctor = item;
					}
					extracted_value = ctor.invoke();
					fromjson_recursively(extracted_value, json_value);
				}
			}

			return extracted_value;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void write_associative_view_recursively(rttr::variant_associative_view& view, rapidjson::Value& json_array_value)
		{
			for (rapidjson::SizeType i = 0; i < json_array_value.Size(); ++i)
			{
				auto& json_index_value = json_array_value[i];
				if (json_index_value.IsObject()) // a key-value associative view
				{
					rapidjson::Value::MemberIterator key_itr = json_index_value.FindMember("key");
					rapidjson::Value::MemberIterator value_itr = json_index_value.FindMember("value");

					if (key_itr != json_index_value.MemberEnd() &&
						value_itr != json_index_value.MemberEnd())
					{
						auto key_var = extract_value(key_itr, view.get_key_type());
						auto value_var = extract_value(value_itr, view.get_value_type());
						if (key_var && value_var)
						{
							view.insert(key_var, value_var);
						}
					}
				}
				else // a key-only associative view
				{
					rttr::variant extracted_value = extract_basic_types(json_index_value);
					if (extracted_value && extracted_value.convert(view.get_key_type()))
						view.insert(extracted_value);
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		void fromjson_recursively(rttr::instance obj2, rapidjson::Value& json_object)
		{
			rttr::instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
			const auto prop_list = obj.get_derived_type().get_properties();

			for (auto prop : prop_list)
			{
				rapidjson::Value::MemberIterator ret = json_object.FindMember(prop.get_name().data());
				if (ret == json_object.MemberEnd())
					continue;
				const rttr::type value_t = prop.get_type();

				auto& json_value = ret->value;
				switch (json_value.GetType())
				{
				case rapidjson::kArrayType:
				{
					rttr::variant var;
					if (value_t.is_sequential_container())
					{
						var = prop.get_value(obj);
						auto view = var.create_sequential_view();
						write_array_recursively(view, json_value);
					}
					else if (value_t.is_associative_container())
					{
						var = prop.get_value(obj);
						auto associative_view = var.create_associative_view();
						write_associative_view_recursively(associative_view, json_value);
					}

					prop.set_value(obj, var);
					break;
				}
				case rapidjson::kObjectType:
				{
					rttr::variant var = prop.get_value(obj);
					fromjson_recursively(var, json_value);
					prop.set_value(obj, var);
					break;
				}
				default:
				{
					rttr::variant extracted_value = extract_basic_types(json_value);
					if (extracted_value.convert(value_t)) // REMARK: CONVERSION WORKS ONLY WITH "const type", check whether this is correct or not!
						prop.set_value(obj, extracted_value);
				}
				}
			}
		}

		void to_json_recursively(const rttr::instance& object, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

		bool write_variant(const rttr::variant& var, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

		//------------------------------------------------------------------------------------------------------------------------
		bool write_atomic_types_to_json(const rttr::type& t, const rttr::variant& var,
			rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
		{
			if (t.is_arithmetic())
			{
				if (t == rttr::type::get<bool>())
					writer.Bool(var.to_bool());
				else if (t == rttr::type::get<char>())
					writer.Bool(var.to_bool());
				else if (t == rttr::type::get<int8_t>())
					writer.Int(var.to_int8());
				else if (t == rttr::type::get<int16_t>())
					writer.Int(var.to_int16());
				else if (t == rttr::type::get<int32_t>())
					writer.Int(var.to_int32());
				else if (t == rttr::type::get<int64_t>())
					writer.Int64(var.to_int64());
				else if (t == rttr::type::get<uint8_t>())
					writer.Uint(var.to_uint8());
				else if (t == rttr::type::get<uint16_t>())
					writer.Uint(var.to_uint16());
				else if (t == rttr::type::get<uint32_t>())
					writer.Uint(var.to_uint32());
				else if (t == rttr::type::get<uint64_t>())
					writer.Uint64(var.to_uint64());
				else if (t == rttr::type::get<float>())
					writer.Double(var.to_double());
				else if (t == rttr::type::get<double>())
					writer.Double(var.to_double());

				return true;
			}
			else if (t.is_enumeration())
			{
				bool ok = false;
				auto result = var.to_string(&ok);
				if (ok)
				{
					writer.String(var.to_string());
				}
				else
				{
					ok = false;
					auto value = var.to_uint64(&ok);
					if (ok)
						writer.Uint64(value);
					else
						writer.Null();
				}

				return true;
			}
			else if (t == rttr::type::get<std::string>())
			{
				writer.String(var.to_string());
				return true;
			}

			return false;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void write_array(const rttr::variant_sequential_view& view, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
		{
			writer.StartArray();
			for (const auto& item : view)
			{
				if (item.is_sequential_container())
				{
					write_array(item.create_sequential_view(), writer);
				}
				else
				{
					rttr::variant wrapped_var = item.extract_wrapped_value();
					rttr::type value_type = wrapped_var.get_type();
					if (value_type.is_arithmetic() || value_type == rttr::type::get<std::string>() || value_type.is_enumeration())
					{
						write_atomic_types_to_json(value_type, wrapped_var, writer);
					}
					else // object
					{
						to_json_recursively(wrapped_var, writer);
					}
				}
			}
			writer.EndArray();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void write_associative_container(const rttr::variant_associative_view& view, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
		{
			static const rttr::string_view key_name("key");
			static const rttr::string_view value_name("value");

			writer.StartArray();

			if (view.is_key_only_type())
			{
				for (auto& item : view)
				{
					write_variant(item.first, writer);
				}
			}
			else
			{
				for (auto& item : view)
				{
					writer.StartObject();
					writer.String(key_name.data(), static_cast<rapidjson::SizeType>(key_name.length()), false);

					write_variant(item.first, writer);

					writer.String(value_name.data(), static_cast<rapidjson::SizeType>(value_name.length()), false);

					write_variant(item.second, writer);

					writer.EndObject();
				}
			}

			writer.EndArray();
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool write_variant(const rttr::variant& var, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
		{
			auto value_type = var.get_type();
			auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
			bool is_wrapper = wrapped_type != value_type;

			if (write_atomic_types_to_json(is_wrapper ? wrapped_type : value_type,
				is_wrapper ? var.extract_wrapped_value() : var, writer))
			{
			}
			else if (var.is_sequential_container())
			{
				write_array(var.create_sequential_view(), writer);
			}
			else if (var.is_associative_container())
			{
				write_associative_container(var.create_associative_view(), writer);
			}
			else
			{
				auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
				if (!child_props.empty())
				{
					to_json_recursively(var, writer);
				}
				else
				{
					bool ok = false;
					auto text = var.to_string(&ok);
					if (!ok)
					{
						writer.String(text);
						return false;
					}

					writer.String(text);
				}
			}

			return true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void to_json_recursively(const rttr::instance& obj2, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
		{
			writer.StartObject();
			rttr::instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;

			auto prop_list = obj.get_derived_type().get_properties();
			for (auto prop : prop_list)
			{
				if (prop.get_metadata("NO_SERIALIZE"))
					continue;

				rttr::variant prop_value = prop.get_value(obj);
				if (!prop_value)
					continue; // cannot serialize, because we cannot retrieve the value

				const auto name = prop.get_name();
				writer.String(name.data(), static_cast<rapidjson::SizeType>(name.length()), false);
				if (!write_variant(prop_value, writer))
				{
					std::cerr << "cannot serialize property: " << name << std::endl;
				}
			}

			writer.EndObject();
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	string_t to_json(rttr::instance object)
	{
		if (!object.is_valid())
		{
			return "";
		}

		rapidjson::StringBuffer sb;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

		to_json_recursively(object, writer);

		return sb.GetString();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool from_json(const string_t& json, rttr::instance object)
	{
		rapidjson::Document document;

		if (document.Parse(json.c_str()).HasParseError())
		{
			return false;
		}

		fromjson_recursively(object, document);

		return true;
	}

} //- io

namespace core
{
	const core::cuuid cuuid::C_INVALID_UUID;

	namespace
	{
		inline static constexpr stringview_t C_EMPTY_STRING = "";

		//------------------------------------------------------------------------------------------------------------------------
		inline static bool is_path_directory(stringview_t path)
		{
			return string_utils::find_substr(path, ".") == MAX(size_t);
		}

		//------------------------------------------------------------------------------------------------------------------------
		inline static bool is_path_file(stringview_t path)
		{
			return string_utils::find_substr(path, ".") != MAX(size_t);
		}

		//- @reference: raylib UnloadFileData.
		//- unload data allocated by load_binary_file_data
		//------------------------------------------------------------------------------------------------------------------------
		inline static void unload_file_binary_data(uint8_t* data)
		{
			mi_free(data);
		}

		//- @reference: raylib LoadFileData. Sevure version. If 'error' is not null then it will be filled with an error message
		//------------------------------------------------------------------------------------------------------------------------
		inline static uint8_t* load_binary_file_data(stringview_t file_path, unsigned* data_size_out, string_t* error)
		{
			uint8_t* data = nullptr;
			*data_size_out = 0;

			if (file_path != nullptr)
			{
				FILE* file = nullptr;

				if (spdlog::details::os::fopen_s(&file, file_path, "rb"))
				{
					fseek(file, 0, SEEK_END);
					auto size = ftell(file);
					fseek(file, 0, SEEK_SET);

					if (size > 0)
					{
						data = (uint8_t*)mi_malloc(sizeof(uint8_t) * size);

						unsigned count = SCAST(unsigned, fread(data, sizeof(uint8_t), size, file));
						*data_size_out = count;
					}
				}
				else
				{
					//- error occured
					if (error)
					{
						error->assign("");
					}
				}
			}
			return data;
		}

		//- @reference: raylib SaveFileData
		//------------------------------------------------------------------------------------------------------------------------
		inline static bool save_binary_file_data(stringview_t file_path, uint8_t* data, unsigned data_size, string_t* error)
		{
			if (file_path != nullptr)
			{
				FILE* file = nullptr;

				if (spdlog::details::os::fopen_s(&file, file_path, "wb"))
				{
					auto count = SCAST(unsigned, fwrite(data, sizeof(uint8_t), data_size, file));

					fclose(file);

					if (count == data_size)
					{
						return true;
					}
				}
				else
				{
					//- error occured
					if (error)
					{
						error->assign("");
					}
				}
			}
			return false;
		}

		//- @reference: raylib UnloadFileText.
		//- unload data allocated by load_text_file_data
		//------------------------------------------------------------------------------------------------------------------------
		inline static void unload_file_text_data(char* text)
		{
			mi_free(text);
		}

		//- @reference: raylib LoadFileText
		//------------------------------------------------------------------------------------------------------------------------
		inline static char* load_text_file_data(stringview_t file_path, string_t* error)
		{
			char* text = nullptr;

			if (file_path != nullptr)
			{
				FILE* file = nullptr;

				if (spdlog::details::os::fopen_s(&file, file_path, "rt"))
				{
					fseek(file, 0, SEEK_END);
					auto size = SCAST(unsigned, ftell(file));
					fseek(file, 0, SEEK_SET);

					if (size > 0)
					{
						text = SCAST(char*, mi_malloc((size + 1) * sizeof(char)));

						if (text != nullptr)
						{
							auto count = SCAST(unsigned, fread(text, sizeof(char), size, file));

							if (count < size)
							{
								text = SCAST(char*, mi_realloc(text, count + 1));
							}

							text[count] = '\0';
						}
					}
					fclose(file);
				}
				else
				{
					//- error occured
					if (error)
					{
						error->assign("");
					}
				}
			}
			return text;
		}

		//- @reference: raylib SaveFileText
		//------------------------------------------------------------------------------------------------------------------------
		inline static bool save_text_file_data(stringview_t file_path, stringview_t text, string_t* error)
		{
			if (file_path != nullptr)
			{
				FILE* file = nullptr;

				if(spdlog::details::os::fopen_s(&file, file_path, "wt"))
				{
					auto count = fprintf(file, "%s", text);

					fclose(file);

					if (count > 0)
					{
						return true;
					}
				}
				else
				{
					//- error occured
					if (error)
					{
						error->assign("");
					}
				}
			}
			return false;
		}

	} //- unnamed

	namespace string_utils
	{
		//------------------------------------------------------------------------------------------------------------------------
		void split(const string_t& string, char delimiter, stl::vector< string_t >& storage)
		{
			std::stringstream ss(string.c_str());
			std::string token;

			while (std::getline(ss, token, delimiter))
			{
				storage.emplace_back(token.c_str());
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
		void erase_substr(string_t& string, const string_t& substring_to_erase)
		{
			auto index = string.find(substring_to_erase);
			if (index != std::string::npos)
			{
				string.erase(index, substring_to_erase.size());
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
			return string.find(substring) != std::string::npos;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool compare(const string_t& first, const string_t& second)
		{
			return first == second;
		}

	} //- string_utils

	namespace detail
	{

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid()
	{
		generate(std::random_device()());
		m_string = generate_string();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(const string_t& uuid)
	{
		parse_string(uuid, m_data);
		m_string = generate_string();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(size_t seed)
	{
		generate(seed);
		m_string = generate_string();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cuuid::cuuid(const cuuid& other)
	{
		copy_from(other);
		m_string = generate_string();
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
		other.m_string = other.generate_string();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cuuid::copy_from(const cuuid& other)
	{
		std::memcpy(&m_data[0], &other.m_data[0], 16);
		m_string = generate_string();
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
		return SCAST(float, m_distribution(C_RANDOM_ENGINE) / MAX(unsigned));
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
	inline int pthread_mutex_lock(pthread_mutex_t* _mutex)
	{
		EnterCriticalSection(_mutex);
		return 0;
	}

	inline int pthread_mutex_unlock(pthread_mutex_t* _mutex)
	{
		LeaveCriticalSection(_mutex);
		return 0;
	}

	inline int pthread_mutex_trylock(pthread_mutex_t* _mutex)
	{
		return TryEnterCriticalSection(_mutex) ? 0 : EBUSY;
	}

	inline int pthread_mutex_init(pthread_mutex_t* _mutex, pthread_mutexattr_t* /*_attr*/)
	{
		InitializeCriticalSection(_mutex);
		return 0;
	}

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
	scolor::scolor(common_color color) :
		m_r(0), m_g(0), m_b(0), m_a(0)
	{
		array_t<uint8_t, 4> C_COMMON_COLORS[] = {
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
		m_string_path = m_path.generic_u8string();
		m_string_ext = m_path.extension().generic_u8string();
		m_string_stem = m_path.stem().generic_u8string();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath::cpath(stringview_t path) :
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
	core::cpath cfilesystem::construct(stringview_t path, stringview_t addition)
	{
		return cpath(path).append(addition);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cpath cfilesystem::join(stringview_t path, stringview_t addition)
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
	bool cfilesystem::create_dir(stringview_t path)
	{
		return std::filesystem::create_directory(path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_dir_in(stringview_t path, stringview_t name)
	{
		return create_dir(cpath(path).append(name).view());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_dir_recursive(stringview_t path)
	{
		return std::filesystem::create_directories(path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_file(stringview_t path)
	{
		std::ofstream out(path);

		std::filesystem::permissions(path,
			std::filesystem::perms::owner_all | std::filesystem::perms::group_all,
			std::filesystem::perm_options::add);

		out.close();

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::create_file_in(stringview_t path, stringview_t stem, stringview_t ext)
	{
		return create_file(fmt::format("{}/{}.{}", path, stem, ext).data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::remove(stringview_t path, stringview_t name)
	{
		cpath p(path);
		p.append(name);

		return remove(p.view());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::remove(stringview_t path)
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
	bool cfilesystem::find_file(stringview_t path, stringview_t name)
	{
		return find_at(path, name, filesystem_lookup_type_file);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_file_by_stem(stringview_t path, stringview_t name)
	{
		cpath p(path);
		if (p.exists())
		{
			for (const auto& entry : std::filesystem::directory_iterator{ p.path() })
			{
				if (entry.is_regular_file() && entry.path().stem().generic_u8string().c_str() == name)
				{
					return true;
				}
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_file_by_extension(stringview_t path, stringview_t name)
	{
		cpath p(path);
		if (p.exists())
		{
			for (const auto& entry : std::filesystem::directory_iterator{ p.path() })
			{
				if (entry.is_regular_file() &&
					entry.path().has_extension() &&
					entry.path().extension().generic_u8string().c_str() == name)
				{
					return true;
				}
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_dir(stringview_t path, stringview_t name)
	{
		return find_at(path, name, filesystem_lookup_type_directory);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cfilesystem::find_at(stringview_t path, stringview_t name, filesystem_lookup_type type)
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
	core::cpath cfilesystem::construct_relative_to_cwd(stringview_t path)
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
	bool cfilesystem::forwards(stringview_t path, bool forced /*= false*/)
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
	void cfilesystem::append(stringview_t path)
	{
		m_current /= path;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfilesystem& cfilesystem::operator/=(stringview_t path)
	{
		append(path);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cfile::cfile(const cpath& path, int mode /*= file_read_write_mode_read | file_read_write_mode_text*/) :
		m_data(nullptr), m_datasize(0), m_mode(mode), m_status(file_io_status_none), m_path(path.view())
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cfile::~cfile()
	{
		if (m_task.valid())
		{
			m_task.wait();
		}
		if (m_data && m_datasize > 0)
		{
			if (!!(m_mode & file_read_write_mode_text))
			{
				//- unload text file data
				unload_file_text_data(SCAST(char*, m_data));
			}
			else if (!!(m_mode & file_read_write_mode_binary))
			{
				//- unload binary file data
				unload_file_binary_data(SCAST(uint8_t*, m_data));
			}
			m_data = nullptr;
			m_datasize = 0;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::spair<void*, unsigned> cfile::take()
	{
		core::spair<void*, unsigned> out;
		out.first = nullptr;
		out.second = 0;

		if (status() == file_io_status_success)
		{
			out.first = m_data;
			out.second = m_datasize;

			//- soft reset to indicate that object should not be used
			m_data = nullptr;
			m_datasize = 0;
			m_mode = file_read_write_mode_none;
			m_error.clear();
			m_status = file_io_status_none;
			m_path = nullptr;

			return out;
		}
		return out;
	}

	//------------------------------------------------------------------------------------------------------------------------
	spair<void*, unsigned> cfile::data() const
	{
		core::spair<void*, unsigned> out;
		out.first = nullptr;
		out.second = 0;

		return out;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::file_io_status cfile::read_sync()
	{
		ASSERT(!!(m_mode & file_read_write_mode_read), "cfile must be created with read mode");

		if (!m_task.valid())
		{
			if (!!(m_mode & file_read_write_mode_text))
			{
				//- load text file
				m_data = SCAST(void*, load_text_file_data(m_path, &m_error));
			}
			else if (!!(m_mode & file_read_write_mode_binary))
			{
				//- load binary file
				m_data = SCAST(void*, load_binary_file_data(m_path, &m_datasize, &m_error));
			}

			if (m_data && m_datasize > 0)
			{
				m_status = file_io_status_success;
			}
			else
			{
				m_status = file_io_status_failed;
			}
		}
		else
		{
			m_task.wait();
		}
		return m_status;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::file_io_status cfile::write_sync(void* data, unsigned data_size)
	{
		ASSERT(!!(m_mode & file_read_write_mode_write), "cfile must be created with write mode");

		if (!m_task.valid())
		{
			if (!!(m_mode & file_read_write_mode_text) && m_data && m_datasize > 0)
			{
				//- write text file data
				m_status = save_text_file_data(m_path, SCAST(char*, m_data), &m_error) == true ? file_io_status_success : file_io_status_failed;
			}
			else if (!!(m_mode & file_read_write_mode_binary) && m_data && m_datasize > 0)
			{
				//- write binary file date
				m_status = save_binary_file_data(m_path, SCAST(uint8_t*, m_data), m_datasize, &m_error) == true ? file_io_status_success : file_io_status_failed;
			}
			else
			{
				m_status = file_io_status_failed;
			}
		}
		else
		{
			m_task.wait();
		}
		return m_status;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::file_io_status cfile::read_async()
	{
		ASSERT(!!(m_mode & file_read_write_mode_read), "cfile must be created with read mode");

		if (!m_task.valid())
		{
			//- begin async load operation
			m_task = casync::launch_async([&]() -> void
				{
					m_status = file_io_status_pending;

					if (!!(m_mode & file_read_write_mode_text))
					{
						//- load text file
						m_data = SCAST(void*, load_text_file_data(m_path, &m_error));
					}
					else if (!!(m_mode & file_read_write_mode_binary))
					{
						//- load binary file
						m_data = SCAST(void*, load_binary_file_data(m_path, &m_datasize, &m_error));
					}
					else
					{
						m_status = file_io_status_failed;
						return;
					}

					if (m_data && m_datasize > 0)
					{
						m_status = file_io_status_success;
					}
					else
					{
						m_status = file_io_status_failed;
					}
				});
		}

		return m_status;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::file_io_status cfile::write_async(void* data, unsigned data_size)
	{
		ASSERT(!!(m_mode & file_read_write_mode_write), "cfile must be created with write mode");

		if (!m_task.valid())
		{
			//- begin async save operation
			m_task = casync::launch_async([&]() -> void
				{
					m_status = file_io_status_pending;

					if (!!(m_mode & file_read_write_mode_text) && m_data && m_datasize > 0)
					{
						//- write text file data
						m_status = save_text_file_data(m_path, SCAST(char*, m_data), &m_error) == true ? file_io_status_success : file_io_status_failed;
					}
					else if (!!(m_mode & file_read_write_mode_binary) && m_data && m_datasize > 0)
					{
						//- write binary file data
						m_status = save_binary_file_data(m_path, SCAST(uint8_t*, m_data), m_datasize, &m_error) == true ? file_io_status_success : file_io_status_failed;
					}
					else
					{
						m_status = file_io_status_failed;
						return;
					}
				});
		}

		return m_status;
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
		ASSERT(started(), "Timer must be started before it can be used");

		auto now = std::chrono::high_resolution_clock::now();

		return SCAST(float, std::chrono::time_point_cast<std::chrono::microseconds>(now).time_since_epoch().count() -
			std::chrono::time_point_cast<std::chrono::microseconds>(m_timepoint).time_since_epoch().count());
	}

	//------------------------------------------------------------------------------------------------------------------------
	cany::~cany()
	{
		if (!empty())
		{
			m_data.reset();
		}
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

} //- core