#pragma once
#include "core_config.hpp"

namespace core
{
	class cservice_manager;

	//- Base class for a service.
	//------------------------------------------------------------------------------------------------------------------------
	class cservice
	{
		friend class cservice_manager;
	public:
		static constexpr service_type_t C_INVALID_TYPE = invalid_handle_t;
		static constexpr unsigned C_SERVICE_COUNT_MAX = 64;

		cservice() = default;
		virtual ~cservice() = default;

		virtual bool on_start() { CORE_ASSERT(false, "Invalid operation. Base class function must be implemented for derived class!"); return false; }
		virtual void on_shutdown() {}
		virtual void on_update(float) {}

		RTTR_ENABLE();
	};

	//- Service manager to be used in-engine. Can hold an arbitrary amount of service types.
	//------------------------------------------------------------------------------------------------------------------------
	class cservice_manager
	{
	public:
		struct sconfig
		{
			vector_t<string_t> m_services;

			RTTR_ENABLE();
		};

		static void init();
		static void shutdown();
		static void on_update(float dt);

		template<class TService>
		static TService& get() { return *find<TService>(); }

		template<class TService>
		static TService* find(stringview_t service_type);

		template<class TService>
		static TService* find();

		static core::cservice* find(const rttr::type& type);

		template<class TService>
		static TService* emplace();

		static bool emplace(rttr::type service_type);

	private:
		inline static int s_service_count = 0;
		inline static service_type_t s_next_type = 0;
		inline static umap_t<size_t, service_type_t> s_service_types;
		inline static array_t<rttr::variant, core::cservice::C_SERVICE_COUNT_MAX> s_services;

	private:
		static core::cservice* get_base_service(rttr::variant& var);
	};

	//- service_type name is required for correct casting of derived classes, i.e.
	//- find<sm::icamera_manager>("ccamera_manager"), where sm::icamera_manager is to what we cast and
	//- "ccamera_manager" is the underlying stored type.
	//- Useful for inheritance indirection, i.e.
	//-
	//- cservice <- derive -- icamera_manager <- derive -- ccamera_manager
	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	TService* cservice_manager::find(stringview_t service_type)
	{
		auto id = rttr::type::get_by_name(service_type.data()).get_id();

		if (s_service_types.find(id) != s_service_types.end())
		{
			auto& var = s_services[s_service_types[id]];

			ASSERT(var.is_valid(), "Invalid operation. Trying to retrieve a released service");

			return reinterpret_cast<TService*>(get_base_service(var));
		}

		ASSERT(false, "Invalid operation. Service does not exist");

		return nullptr;
	}

	//- cast to given service type without inheritance indirection.
	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	TService* cservice_manager::find()
	{
		auto id = rttr::type::get<TService>().get_id();

		if (s_service_types.find(id) != s_service_types.end())
		{
			auto& var = s_services[s_service_types[id]];

			ASSERT(var.is_valid(), "Invalid operation. Trying to retrieve a released service");

			return reinterpret_cast<TService*>(get_base_service(var));
		}

		ASSERT(false, "Invalid operation. Service does not exist");

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	TService* cservice_manager::emplace()
	{
		if (s_next_type < core::cservice::C_SERVICE_COUNT_MAX)
		{
			auto type = rttr::type::get<TService>();
			auto id = type.get_id();

			auto t = s_next_type++;
			s_services[t] = type.create({});
			s_service_types[id] = t;
			s_service_count++;
			return reinterpret_cast<TService*>(get_base_service(s_services[t]));
		}

		ASSERT(false, "Invalid operation. Maximum Service count reached, increase the count in 'core' library");

		return nullptr;
	}

} //- core