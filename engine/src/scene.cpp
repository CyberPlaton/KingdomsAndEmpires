#include "scene.hpp"

namespace engine
{
	const std::string& cscene_base::entities() const
	{
		return {};
	}
	
	const std::string& cscene_base::modules() const
	{
		return {};
	}
	
	const std::string& cscene_base::systems() const
	{
		return {};
	}
	
	
	//------------------------------------------------------------------------------------------------------------------------
	bool scene_properties::has(const std::string& component) const
	{
		auto h = algorithm::hash(component.c_str());

		return m_props.find(h) != m_props.end();
	}

	//------------------------------------------------------------------------------------------------------------------------
	const rttr::variant& scene_properties::at(const std::string& component) const
	{
		auto h = algorithm::hash(component.c_str());

		return m_props.at(h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	rttr::variant& scene_properties::operator[](const std::string& component)
	{
		auto h = algorithm::hash(component.c_str());

		//- check if property does exist, if not,
		//- try emplacing a variant with expected type if it is valid
		if (m_props.find(h) != m_props.end())
		{
			return m_props[h];
		}

		auto expected = rttr::type::get_by_name(component);

		if (expected.is_valid())
		{
			m_props[h] = expected.create();

			return m_props[h];
		}

		ASSERT(false, "Invalid operation. Scene property with invalid type could not be constructed");
	}

	//------------------------------------------------------------------------------------------------------------------------
	cscene::cscene() :
		m_status(scene_status_none)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cscene::~cscene()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	engine::scene_status cscene::status() const
	{
		return m_status;
	}

	//------------------------------------------------------------------------------------------------------------------------
	const engine::cscene_entity* cscene::at(const core::cuuid& uuid) const
	{
		if (auto it = m_lookup.find(uuid.hash()); it != m_lookup.end())
		{
			return find(it->second);
		}
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	engine::cscene_entity* cscene::operator[](const core::cuuid& uuid)
	{
		if (auto it = m_lookup.find(uuid.hash()); it != m_lookup.end())
		{
			return modify(it->second);
		}

		uint64_t i = 0;

		auto* node = append_to(nullptr, &i);

		m_lookup[uuid.hash()] = i;

		return node;
	}

	//------------------------------------------------------------------------------------------------------------------------
	scene_status cscene::load(const std::string& path)
	{
		using namespace std::chrono_literals;

		if (!m_loading_task.valid())
		{
			m_status = scene_status_loading;
			m_loading_task = core::cfile::load_text_async(path);
		}
		else
		{
			auto status = m_loading_task.wait_for(1ms);

			if (status == std::future_status::ready)
			{
				m_status = scene_status_loaded;

				auto text = m_loading_task.get();

				m_json = nlohmann::json::parse(text);
			}
			else if (status == std::future_status::timeout)
			{
				m_status = scene_status_failed;
			}
		}
		return m_status;
	}

	//------------------------------------------------------------------------------------------------------------------------
	scene_status cscene::resolve()
	{
		m_status = scene_status_resolving;

		for (auto& e : m_json.at("entities"))
		{
			resolve_scene_entity(e);
		}

		m_status = scene_status_resolved;

		return m_status;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cscene::resolve_scene_entity(nlohmann::json& json)
	{
		//-- NOTE: JSON throws when keys are not present or type is wrong
		auto uuid = json["uuid"].template get<std::string>();
		auto name = json["name"].template get<std::string>();

		for (auto& c : json["components"])
		{
			const auto& component_string = c.dump();

			logging::log_debug(fmt::format("\tc: '{}'", component_string));
		}
	}

} //- engine