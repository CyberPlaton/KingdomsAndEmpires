#pragma once
#include <core.h>

namespace engine
{
	class cservice_manager;

	using service_type_t = handle_type_t;

	//- base class for a service
	//------------------------------------------------------------------------------------------------------------------------
	class cservice
	{
		friend class cservice_manager;
	public:
		inline static const service_type_t C_INVALID_TYPE = invalid_handle_t;
		inline static const unsigned C_SERVICE_COUNT_MAX = 64;

		cservice() = default;
		virtual ~cservice() = default;

		virtual bool on_start() {return false;}
		virtual void on_shutdown() {}
		virtual void on_update(float) {}

		RTTR_ENABLE();
	};

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
		static TService& modify();

		template<class TService>
		static const TService& find();

		template<class TService>
		static TService& emplace();

		template<class TService, typename... ARGS>
		static TService& emplace(ARGS&&... args);

		static bool emplace(rttr::type service_type);

		template<class TService>
		static void release();

	private:
		inline static handle_type_t s_service_count = 0;
		inline static service_type_t s_next_type = 0;
		inline static umap_t<size_t, service_type_t> s_service_types;
		inline static array_t<rttr::variant, cservice::C_SERVICE_COUNT_MAX> s_services;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	TService& cservice_manager::modify()
	{
		static_assert(std::is_base_of<cservice, TService>::value, "TService is required to be derived from cservice");

		auto id = rttr::type::get<TService>().get_id();

		if (s_service_types.find(id) != s_service_types.end())
		{
			return s_services[s_service_types[id]].get_value<ref_t<TService>>();
		}
		ASSERT(false, "Invalid operation. Service does not exist");
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	const TService& cservice_manager::find()
	{
		static_assert(std::is_base_of<cservice, TService>::value, "TService is required to be derived from cservice");

		auto id = rttr::type::get<TService>().get_id();

		if (s_service_types.find(id) != s_service_types.end())
		{
			return s_services[s_service_types[id]].get_value<ref_t<TService>>();
		}
		ASSERT(false, "Invalid operation. Service does not exist");
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	void cservice_manager::release()
	{
		static_assert(std::is_base_of<cservice, TService>::value, "TService is required to be derived from cservice");

		auto id = rttr::type::get<TService>().get_id();

		if (s_service_types.find(id) != s_service_types.end())
		{
			const auto t = s_service_types[id];
			s_services[t].clear();
			s_service_types.erase(id);
			s_service_count = s_service_count - 1 < 0 ? 0 : s_service_count - 1;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	TService& cservice_manager::emplace()
	{
		static_assert(std::is_base_of<cservice, TService>::value, "TService is required to be derived from cservice");

		if (s_next_type < detail::iservice::C_SERVICE_COUNT_MAX)
		{
			auto id = rttr::type::get<TService>().get_id();

			auto t = s_next_type++;
			s_services[t] = TService(t);
			s_service_types[id] = t;
			s_service_count++;
			return s_services[t].get_value<ref_t<TService>>();
		}
		ASSERT(false, "Invalid operation. Service does not exist");
	}

	//------------------------------------------------------------------------------------------------------------------------
	template< class TService, typename... ARGS>
	TService& cservice_manager::emplace(ARGS&&... args)
	{
		static_assert(std::is_base_of<cservice, TService>::value, "TService is required to be derived from cservice");

		if (s_next_type < detail::iservice::C_SERVICE_COUNT_MAX)
		{
			auto id = rttr::type::get<TService>().get_id();

			auto t = s_next_type++;
			s_services[t] = TService(t, args...);
			s_service_types[id] = t;
			s_service_count++;
			return s_services[t].get_value<ref_t<TService>>();
		}
		ASSERT(false, "Invalid operation. Service does not exist");
	}

} //- engine

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cservice)
	{
		rttr::registration::class_<cservice>("cservice")
			.constructor<>()
			.method("on_start", &cservice::on_start)
			.method("on_shutdown", &cservice::on_shutdown)
			.method("on_update", &cservice::on_update)
			;
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cservice_manager::sconfig)
	{
		rttr::registration::class_<cservice_manager::sconfig>("cservice_manager::sconfig")
			.property("m_services", &cservice_manager::sconfig::m_services)
			;
	}

} //- engine