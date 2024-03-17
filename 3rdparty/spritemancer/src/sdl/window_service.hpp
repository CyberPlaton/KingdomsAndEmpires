#pragma once
#include "sdl_service.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class cwindow_service final : public core::cservice
	{
	public:
		cwindow_service() = default;
		~cwindow_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float dt) override final;

	private:
		RTTR_ENABLE(core::cservice);
	};

} //- sm

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cwindow_service)
	{
		rttr::registration::class_<cwindow_service>("cwindow_service")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

} //- sm