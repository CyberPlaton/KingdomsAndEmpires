#pragma once
#include "sm_config.hpp"
#include "sm_buffer.hpp"

namespace sm
{
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

} //- sm