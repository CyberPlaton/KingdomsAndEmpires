#pragma once
#include "sm_config.hpp"
#include "sm_buffer.hpp"

namespace sm
{
	struct smesh;

	namespace mesh
	{


	} //- mesh

	//------------------------------------------------------------------------------------------------------------------------
	struct smesh
	{
		sbuffer m_buffer;
	};

	//- A mesh contains arbitrary geometry and handles to buffers.
	//------------------------------------------------------------------------------------------------------------------------
	class cmesh : public core::cresource
	{
	public:
		explicit cmesh();
		explicit cmesh(rttr::type vertex_type);

		void bind();

		opresult create(rttr::type vertex_type);
		opresult create();

		inline const detail::cbuffer& buffer() const { return m_buffer; }
		inline detail::cbuffer& buffer() { return m_buffer; }

	private:
		detail::cbuffer m_buffer;

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