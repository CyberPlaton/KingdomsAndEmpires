#pragma once
#include "core_config.hpp"
#include "core_algorithm.hpp"
#include "core_mutex.hpp"
#include "core_filesystem.hpp"
#include "core_resource.hpp"
#include "core_thread_service.hpp"

namespace core
{
	namespace resource
	{
		using edit_pair_t = std::pair<bool, iresource*>;

		//- Resource manager abstract class.
		//------------------------------------------------------------------------------------------------------------------------
		class iresource_manager
		{
		public:
			iresource_manager() = default;
			virtual ~iresource_manager() = default;

			virtual edit_pair_t lock_and_edit(stringview_t) = 0;
			virtual edit_pair_t lock_and_edit(const iresource*) = 0;

			virtual void unlock(stringview_t) = 0;
			virtual void unlock(iresource*) = 0;

			virtual const iresource* find(stringview_t name) const = 0;

		private:
			RTTR_ENABLE();
		};

	} //- resource

	//- Interface class for a resource manager.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	class cresource_manager :
		public cservice,
		public resource::iresource_manager
	{
	protected:
		virtual const resource::iresource* load(stringview_t, const fs::cfileinfo&) = 0;

		const resource::iresource* emplace_object(stringview_t name, TResource&& res);

	public:
		cresource_manager() = default;
		virtual ~cresource_manager() = default;

		virtual bool on_start() override { return true; }
		virtual void on_shutdown() override {}
		virtual void on_update(float) override {}

		//- Lock the resource in question for editing. This allows for modifying operations on the actual object.
		//- In case the object is locked, then this returns false.
		resource::edit_pair_t lock_and_edit(stringview_t name) override final;

		resource::edit_pair_t lock_and_edit(const resource::iresource* res) override final;

		void unlock(stringview_t name) override final;

		void unlock(resource::iresource* res) override final;

		const TResource* load_sync(stringview_t name, const fs::cfileinfo& path);

		const TResource* load_async(stringview_t name, const fs::cfileinfo& path);

	private:
		umap_t<unsigned, TResource> m_data;
		core::cmutex m_mutex;

	private:
		const TResource* do_load_async(stringview_t name, const fs::cfileinfo& path);

		RTTR_ENABLE(cservice, resource::iresource_manager);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	const resource::iresource* core::cresource_manager<TResource>::emplace_object(stringview_t name, TResource&& res)
	{
		const auto h = algorithm::hash(name);

		const auto& object = m_data.emplace(h, std::move(res));

		return &object;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	const TResource* core::cresource_manager<TResource>::do_load_async(stringview_t name, const fs::cfileinfo& path)
	{
		auto* ts = core::cservice_manager::find<core::cthread_service>();

		//- Execute a function directly in async, returning only the future
		tf::Future<TResource*> future = ts->push_background_job(fmt::format("do_load_async '{}'", name),
			[n = name, p = path]() -> const TResource*
			{
				return static_cast<TResource*>(load(n, p));
			});

		//- Directly usable API from iresource to watch over resource status
		//- and optionally retrieve some read-only data as soon as possible
		return future.get();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	const TResource* core::cresource_manager<TResource>::load_async(stringview_t name, const fs::cfileinfo& path)
	{
		core::cscope_mutex m(m_mutex);

		return static_cast<TResource*>(do_load_async(name, path));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	const TResource* core::cresource_manager<TResource>::load_sync(stringview_t name, const fs::cfileinfo& path)
	{
		core::cscope_mutex m(m_mutex);

		return static_cast<TResource*>(load(name, path));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	void core::cresource_manager<TResource>::unlock(stringview_t name)
	{
		const auto h = algorithm::hash(name);

		unlock(const_cast<iresource*>(&m_data.at(h)));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	void core::cresource_manager<TResource>::unlock(resource::iresource* res)
	{
		res->unlock();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	resource::edit_pair_t core::cresource_manager<TResource>::lock_and_edit(stringview_t name)
	{
		const auto h = algorithm::hash(name);

		return lock_and_edit(&m_data.at(h));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	resource::edit_pair_t core::cresource_manager<TResource>::lock_and_edit(const resource::iresource* res)
	{
		auto* r = const_cast<iresource*>(res);

		if (!r->locked() && r.lock())
		{
			return { true, r };
		}

		//- Failed to lock resource, means it is currently being edited by someone else.
		return { false, nullptr };
	}

} //- core
