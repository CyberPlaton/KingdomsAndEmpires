#pragma once
#include <engine.h>

namespace camera_system
{
	//- stack-machine-kind camera manager
	//------------------------------------------------------------------------------------------------------------------------
	class ccamera_manager : public engine::cservice
	{
	public:
		ccamera_manager() = default;
		~ccamera_manager() = default;

		bool on_start() override final{return true;}
		void on_shutdown() override final {}
		void on_update(float dt) override final {}
		
// 		template< class T >
// 		T* push_camera(const string_t& name);
// 		void pop_camera(const string_t& name);
// 		void set_default_camera(const string_t& name);
// 		template< class T >
// 		void set_default_camera(const string_t& name);
// 		void set_camera_active(const string_t& name);
// 		bool is_camera_active(const string_t& name);
// 		sm::ccamera* get_active() const;
// 		sm::ccamera* get_default() const;
// 		sm::ccamera* find(const string_t& name) const;
// 		template< class T >
// 		T* find_as(const string_t& name) const;
// 		template< class T >
// 		T* get_active_as() const;
// 		template< class T >
// 		T* get_default_as() const;
// 		bool has_default_camera() const;
// 		bool has_active_camera() const;

	private:
		//umap_t<unsigned, ptr_t<sm::ccamera> > m_cameras;
		unsigned m_default_camera = 0;
		unsigned m_active_camera = 0;

		RTTR_ENABLE(engine::cservice);
		REFLECTABLE();
	};

} //- camera_system

namespace camera_system
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(ccamera_manager)
	{
		rttr::registration::class_<ccamera_manager>("ccamera_manager")
			.constructor<>()
// 			.method("on_start", &ccamera_manager::on_start)
// 			.method("on_shutdown", &ccamera_manager::on_shutdown)
// 			.method("on_update", &ccamera_manager::on_update)
			;
	}

} //- camera_system