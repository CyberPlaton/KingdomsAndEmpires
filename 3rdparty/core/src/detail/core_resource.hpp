#pragma once
#include "core_config.hpp"

namespace core
{
	namespace resource
	{
		class iresource_manager;

		//- Resource interface class.
		//------------------------------------------------------------------------------------------------------------------------
		class iresource
		{
			friend class iresource_manager;
		public:
			iresource() : m_acquired(false), m_status(core::resource_status_none) {}
			virtual ~iresource() = default;

			inline bool locked() const { return m_acquired.load(std::memory_order_acquire); };
			inline bool ready() const { return status() == core::resource_status_ready; }
			inline core::resource_status status() const { return m_status; }

		protected:
			std::atomic_bool m_acquired;
			core::resource_status m_status;

		private:
			inline bool lock() { m_acquired.store(true, std::memory_order_acquire); };
			inline void unlock() { m_acquired.store(false, std::memory_order_release); };

			RTTR_ENABLE();
		};

	} //- resource

	//- Resource base class.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	class cresource : public resource::iresource
	{
	public:
		cresource() = default;
		virtual ~cresource() = default;

	public:
		TResource m_resource;

		RTTR_ENABLE(resource::iresource);
	};

} //- core