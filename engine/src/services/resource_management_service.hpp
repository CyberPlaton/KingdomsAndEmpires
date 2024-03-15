#pragma once
#include <core.h>

namespace engine
{
	//- Central class for loading and unloading (i.e. managing) resources.
	//- TODO: handle default engine/editor resources.
	//------------------------------------------------------------------------------------------------------------------------
	class cresource_management_service final : public core::cservice
	{
	public:
		struct sconfig
		{
			vector_t<std::string> m_managers;

			RTTR_ENABLE();
		};

		cresource_management_service() = default;
		~cresource_management_service() = default;

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float dt) override final;

		auto managers() const {return m_manager_count;}

		template<class TManagerType>
		TManagerType* find();

		template<class TManagerType>
		TManagerType* emplace();

		bool emplace(rttr::type manager_type);

	private:
		int m_manager_count = 0;
		handle_type_t m_next_type = 0;
		umap_t<size_t, handle_type_t> m_manager_types;
		array_t<rttr::variant, core::iresource_manager::C_MANAGER_COUNT_MAX> m_managers;

		RTTR_ENABLE(core::cservice);

	private:
		static core::iresource_manager* get_base_manager(rttr::variant& var);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<class TManagerType>
	TManagerType* cresource_management_service::find()
	{
		auto id = rttr::type::get<TManagerType>().get_id();

		if (m_manager_types.find(id) != m_manager_types.end())
		{
			auto& var = m_managers[m_manager_types[id]];

			ASSERT(var.is_valid(), "Invalid operation. Trying to retrieve a released manager");

			return reinterpret_cast<TManagerType*>(get_base_manager(var));
		}

		ASSERT(false, "Invalid operation. Manager does not exist");

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TManagerType>
	TManagerType* cresource_management_service::emplace()
	{
		if (m_next_type < core::iresource_manager::C_MANAGER_COUNT_MAX)
		{
			auto type = rttr::type::get<TManagerType>();
			auto id = type.get_id();

			auto t = m_next_type++;
			m_managers[t] = type.create({});
			m_manager_types[id] = t;
			m_manager_count++;
			return reinterpret_cast<TManagerType*>(get_base_manager(m_managers[t]));
		}

		ASSERT(false, "Invalid operation. Maximum Manager count reached, increase the count in 'core' library");

		return nullptr;
	}

} //- engine

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cresource_management_service::sconfig)
	{
		rttr::registration::class_<cresource_management_service::sconfig>("cresource_management_service::sconfig")
			.property("m_managers", &cresource_management_service::sconfig::m_managers)
			;

		rttr::default_constructor<cresource_management_service::sconfig>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cresource_management_service)
	{
		rttr::registration::class_<cresource_management_service>("cresource_management_service")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

} //- engine