#pragma once
#include "core_config.hpp"
#include "core_future_type.hpp"
#include "core_algorithm.hpp"
#include "core_async.hpp"
#include "core_mutex.hpp"

namespace core
{
	//- Base class for a resource. When creating a new resource inherit from this class and implement any required functionality,
	//- and reflect with RTTR. This will also help to verify a class when registering a new resource
	//------------------------------------------------------------------------------------------------------------------------
	class cresource
	{
	public:
		static constexpr stringview_t C_META_SUPPORTED_EXTENSIONS	= "RESOURCE_EXTENSIONS";

		cresource() = default;
		virtual ~cresource() = default;

		RTTR_ENABLE();
	};

	//- Base class for a resource manager. Data is not serialized, the class serves to avoid redefining functionality
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	class cresource_manager
	{
	public:
		virtual ~cresource_manager() = default;

		bool lookup(stringview_t name) const;
		bool lookup(handle_type_t handle) const;

		TResource& get(stringview_t name);
		const TResource& at(stringview_t name) const;

		TResource& get(handle_type_t handle);
		const TResource& at(handle_type_t handle) const;

		template<typename TCallable>
		void each(TCallable&& callback);

		unsigned count() const { return static_cast<unsigned>(m_data.size()); }

		void erase(stringview_t name) { erase(algorithm::hash(name)); }
		void erase(handle_type_t handle) { m_data.erase(handle); }
		void clear() { m_data.clear(); }
		handle_type_t handle(stringview_t name) { return algorithm::hash(name); }

	protected:
		umap_t<unsigned, TResource> m_data;
		cmutex m_mutex;

	protected:
		//- Utility function to load a resource synchronously and construct it in-place with given arguments.
		//------------------------------------------------------------------------------------------------------------------------
		template<typename THandle, typename... ARGS>
		THandle load_of_sync(const char* name, ARGS&&... args)
		{
			core::cscope_mutex m(m_mutex);

			unsigned hash = algorithm::hash(name);

			//- correctly forward arguments and hash
			m_data.emplace(std::piecewise_construct,
				std::forward_as_tuple(hash),
				std::forward_as_tuple(std::forward<ARGS>(args)...));

			return THandle(hash);
		}

		//- Utility function to load a resource asynchronously and construct it in-place with given arguments. Returns a future that
		//- will notify when resource handle is ready.
		//------------------------------------------------------------------------------------------------------------------------
		template<typename THandle, typename... ARGS>
		core::cfuture_type<THandle> load_of_async(const char* name, ARGS&&... args)
		{
			core::cfuture_type<THandle> result = core::casync::launch_async([&]() -> THandle
				{
					core::cscope_mutex m(m_mutex);

					const auto hash = algorithm::hash(name);

					//- correctly forward arguments and hash
					m_data.emplace(std::piecewise_construct,
						std::forward_as_tuple(hash),
						std::forward_as_tuple(std::forward<ARGS>(args)...));

					return hash;

				}).share();

			return result;
		}

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	template<typename TCallable>
	void core::cresource_manager<TResource>::each(TCallable&& callback)
	{
		core::cscope_mutex m(m_mutex);

		for (const auto& pair : m_data)
		{
			callback(pair.second);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	const TResource& core::cresource_manager<TResource>::at(stringview_t name) const
	{
		return at(algorithm::hash(name));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	TResource& core::cresource_manager<TResource>::get(stringview_t name)
	{
		return get(algorithm::hash(name));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	const TResource& core::cresource_manager<TResource>::at(handle_type_t handle) const
	{
		return m_data.at(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	TResource& core::cresource_manager<TResource>::get(handle_type_t handle)
	{
		return m_data[handle];
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	bool core::cresource_manager<TResource>::lookup(stringview_t name) const
	{
		return lookup(algorithm::hash(name));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	bool core::cresource_manager<TResource>::lookup(handle_type_t handle) const
	{
		return m_data.find(handle) != m_data.end();
	}

} //- core
