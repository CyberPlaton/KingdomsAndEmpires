#pragma once
#include <core.h>

namespace engine
{
	class cservice_manager;

	using service_type_t = handle_type_t;

	//------------------------------------------------------------------------------------------------------------------------
	enum service_start_phase : uint8_t
	{
		service_start_phase_none = 0,
		service_start_phase_pre_init,	//- init 'directly' after registration
		service_start_phase_init,		//- init after normal init was done
		service_start_phase_post_init,	//- init before entering run loop
	};

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		class iservice
		{
		public:
			inline static const service_type_t C_INVALID_TYPE = invalid_handle_t;
			inline static const unsigned C_SERVICE_COUNT_MAX = 64;

			explicit iservice(service_type_t type, service_start_phase start_phase = service_start_phase_init) :
				m_type(type), m_start_phase(start_phase)
			{
			}
			virtual ~iservice() = default;

			virtual bool on_start() = 0;
			virtual void on_shutdown() = 0;
			virtual void on_update(float) = 0;

			inline service_type_t type() const {return m_type;}
			inline service_start_phase phase() const {return m_start_phase;}

		private:
			const service_type_t m_type;
			const service_start_phase m_start_phase;
		};

	} //- detail

	//- base class for a service
	//------------------------------------------------------------------------------------------------------------------------
	class cservice : public detail::iservice
	{
		friend class cservice_manager;
	public:
		virtual ~cservice() = default;

		virtual void on_shutdown() {}
		virtual void on_update(float) {}

	protected:
		cservice(service_type_t type, service_start_phase start_phase = service_start_phase_init) :
			detail::iservice(type, start_phase)
		{
		}

	private:
		cservice() = default;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cservice_manager
	{
	public:
		static void init(service_start_phase start_phase);
		static void shutdown();
		static void on_update(float dt);

		template<class TService>
		static TService* modify();

		template<class TService>
		static const TService* find();

		template<class TService>
		static TService* emplace();

		template<class TService, typename... ARGS>
		static TService* emplace(ARGS&&... args);

		template<class TService>
		static void release();

	private:
		static handle_type_t s_service_count;
		static service_type_t s_next_type;
		static umap_t<unsigned, service_type_t> s_service_types;
		static array_t<ptr_t<cservice>, detail::iservice::C_SERVICE_COUNT_MAX> s_services;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	TService* cservice_manager::modify()
	{
		static_assert(std::is_base_of<cservice, TService>::value, "TService is required to be derived from cservice");

		if (s_service_types.find(typeid(TService).hash_code()) != s_service_types.end())
		{
			return reinterpret_cast<TService*>(s_services[s_service_types[typeid(TService).hash_code()]].get());
		}
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	const TService* cservice_manager::find()
	{
		static_assert(std::is_base_of<cservice, TService>::value, "TService is required to be derived from cservice");

		if (s_service_types.find(typeid(TService).hash_code()) != s_service_types.end())
		{
			return reinterpret_cast<TService*>(s_services[s_service_types[typeid(TService).hash_code()]].get());
		}
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	void cservice_manager::release()
	{
		static_assert(std::is_base_of<cservice, TService>::value, "TService is required to be derived from cservice");

		if (s_service_types.find(typeid(TService).hash_code()) != s_service_types.end())
		{
			const auto t = s_service_types[typeid(TService).hash_code()];
			s_services[t].reset();
			s_services[t] = nullptr;
			s_service_types.erase(typeid(TService).hash_code());
			s_service_count = s_service_count - 1 < 0 ? 0 : s_service_count - 1;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	TService* cservice_manager::emplace()
	{
		static_assert(std::is_base_of<cservice, TService>::value, "TService is required to be derived from cservice");
		if (s_next_type < detail::iservice::C_SERVICE_COUNT_MAX)
		{
			auto t = s_next_type++;
			s_services[t] = std::move(std::make_unique<TService>(t));
			s_service_types[typeid(TService).hash_code()] = t;
			s_service_count++;
			return reinterpret_cast<TService*>(s_services[t].get());
		}
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template< class TService, typename... ARGS>
	TService* cservice_manager::emplace(ARGS&&... args)
	{
		static_assert(std::is_base_of<cservice, TService>::value, "TService is required to be derived from cservice");
		if (s_next_type < detail::iservice::C_SERVICE_COUNT_MAX)
		{
			auto t = s_next_type++;
			s_services[t] = std::move(std::make_unique<TService>(t, args...));
			s_service_types[typeid(TService).hash_code()] = t;
			s_service_count++;
			return reinterpret_cast<TService*>(s_services[t].get());
		}
		return nullptr;
	}

} //- engine