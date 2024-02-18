#pragma once
#include <core.h>
#include "ecs/ecs_world.hpp"

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	enum scene_entity_type
	{
		scene_entity_type_none = 0,
		scene_entity_type_entity,
		scene_entity_type_prefab,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum scene_status
	{
		scene_status_none = 0,
		scene_status_loading,	//- loading JSON resource from file
		scene_status_loaded,	//- finished loading JSON resource
		scene_status_resolving,	//- processing JSON into scene tree
		scene_status_resolved,	//- JSON resource processed and converted into scene tree
		scene_status_failed,	//- either failed JSON loading or converting to scene tree
	};

	//- basically pairing component rttr names to component data. The variant is easily castable to actual
	//- component type, making a back and forward between runtime and serializable state easier.
	//------------------------------------------------------------------------------------------------------------------------
	class scene_properties
	{
	public:
		scene_properties() = default;
		~scene_properties() = default;

		bool has(const std::string& component) const;

		template<class TType>
		bool has() const;

		const rttr::variant& at(const std::string& component) const;
		rttr::variant& operator[](const std::string& component);

		template<class TProperty>
		TProperty* find();

		template<class TProperty>
		const TProperty* find() const;

		decltype(auto) begin(){ return m_props.begin(); }
		decltype(auto) end() { return m_props.end(); }

	private:
		umap_t<unsigned, rttr::variant> m_props;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cscene_entity
	{
	public:

	private:
		scene_properties m_props;
		core::cuuid m_uuid;

	};

	//- intermediate format between serializeable state and runtime world.
	//- Note: this is a base class, intended to be specialized into a 'Prefab' and 'World' scene.
	//------------------------------------------------------------------------------------------------------------------------
	class cscene : public core::clinked_tree<cscene_entity>
	{
	public:
		cscene();
		virtual ~cscene();

		const cscene_entity* at(const core::cuuid& uuid) const;
		cscene_entity* operator[](const core::cuuid& uuid);

		scene_status load(const std::string& path);
		virtual scene_status resolve();

		scene_status status() const;

	protected:
		inline nlohmann::json& json() { return m_json; }

	private:
		umap_t<unsigned, uint64_t> m_lookup;
		nlohmann::json m_json;
		std::future<std::string> m_loading_task;
		scene_status m_status;

	private:
		void resolve_scene_entity(nlohmann::json& json);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<class TType>
	bool engine::scene_properties::has() const
	{
		return has(rttr::type::get<TType>());
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TProperty>
	TProperty* engine::scene_properties::find()
	{
		auto h = algorithm::hash(rttr::type::get<TProperty>().get_name().data());

		if (m_props.find(h) != m_props.end())
		{
			return RCAST(TProperty*, &m_props.at(h).get_value<TProperty>());
		}
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TProperty>
	const TProperty* engine::scene_properties::find() const
	{
		auto h = algorithm::hash(rttr::type::get<TProperty>().get_name().data());

		if (m_props.find(h) != m_props.end())
		{
			return RCAST(TProperty*, &m_props.at(h).get_value<TProperty>());
		}
		return nullptr;
	}

} //- engine