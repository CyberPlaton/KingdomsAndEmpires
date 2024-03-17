#pragma once
#include "sdl_service.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class cinput_service final : public core::cservice
	{
	public:
		cinput_service() = default;
		~cinput_service();

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
	REFLECT_INLINE(cinput_service)
	{
		rttr::registration::class_<cinput_service>("cinput_service")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			;
	}

} //- sm