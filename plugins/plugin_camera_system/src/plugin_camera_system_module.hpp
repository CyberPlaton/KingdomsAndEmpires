#pragma once
#include <engine.h>
#include <plugin_logging.h>

namespace camera_system
{
	//- stack-machine-kind camera manager
	//------------------------------------------------------------------------------------------------------------------------
	class ccamera_manager : public sm::icamera_manager
	{
	public:
		ccamera_manager();
		~ccamera_manager() = default;

		bool on_start() override final{return true;}
		void on_shutdown() override final {}
		void on_update(float dt) override final {}

		template <class TCamera>
		TCamera* push(stringview_t name, bool set_active = false);

		void pop(stringview_t name);

		sm::ccamera* default_camera() const override final;

		sm::ccamera* active_camera() const override final;

		bool has_active_camera() const override final;

	private:
		umap_t<unsigned, ptr_t<sm::ccamera>> m_cameras;
		ptr_t<sm::ccamera> m_default;
		unsigned m_active_camera = 0;

		RTTR_ENABLE(sm::icamera_manager);
		REFLECTABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	template <class TCamera>
	TCamera* camera_system::ccamera_manager::push(stringview_t name, bool set_active /*= false*/)
	{
		auto h = algorithm::hash(name);

		if (m_cameras.find(h) == m_cameras.end())
		{
			m_cameras[h] = std::make_unique<TCamera>();

			if (set_active)
			{
				m_active_camera = h;
			}

			return reinterpret_cast<TCamera*>(m_cameras.at(h).get());
		}
		else
		{
			logging::log_warn(fmt::format("Trying to push a camera of type '{}' with existing name '{}'",
				rttr::type::get<TCamera>().get_name().data(), name));
		}
		ASSERT(false, "Failed pushing camera");
		return nullptr;
	}

} //- camera_system

namespace camera_system
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(ccamera_manager)
	{
		rttr::registration::class_<ccamera_manager>("ccamera_manager")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

} //- camera_system