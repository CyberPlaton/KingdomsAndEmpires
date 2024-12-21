#pragma once
#include "sm_config.hpp"
#include "sm_buffer.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	enum geometry_type : uint8_t
	{
		geometry_type_none = 0,
		geometry_type_static,
		geometry_type_dynamic,
		geometry_type_transient,
	};

	//- A mesh contains arbitrary geometry and handles to buffers.
	//------------------------------------------------------------------------------------------------------------------------
	class cmesh : public core::cresource
	{
	public:
		explicit cmesh();
		explicit cmesh(geometry_type geometry, primitive_type primitive);
		~cmesh();

		//- Note: when creating we expect that the buffer is filled with data. Otherwise nothing will
		//- actually be created. So it is perfectly reasonable to create an empty mesh to acquire the handle
		//- and afterwards use update to fill buffers with updated data.
		void create(geometry_type geometry, primitive_type primitive);

		//- Note: updating a static buffer will recreate the buffer with new geometry. A dynamic buffer will
		//- update the existing buffer with new vertex data and transient buffer wont do anything.
		void update();

		//- Note: binding a transient buffer will create a transient buffer with current vertex data.
		void bind();

		inline const sbuffer& buffer() const { return m_buffer; }
		inline sbuffer& buffer() { return m_buffer; }

	private:
		sbuffer m_buffer;

		RTTR_ENABLE(core::cresource);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cmesh_manager final :
		public core::cservice,
		public core::cresource_manager<cmesh>
	{
	public:
		cmesh_manager(unsigned reserve = C_MESH_RESOURCE_MANAGER_RESERVE_COUNT);
		~cmesh_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		mesh_handle_t load_sync(stringview_t name, rttr::type vertex_type);
		mesh_handle_t load_sync(stringview_t name);

		core::cfuture_type<mesh_handle_t> load_async(stringview_t name, rttr::type vertex_type);
		core::cfuture_type<mesh_handle_t> load_async(stringview_t name);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<cmesh>);
	};

} //- sm