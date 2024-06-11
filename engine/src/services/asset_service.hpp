#pragma once
#include "../assets/asset.hpp"
#include "../assets/resource_compiler.hpp"

namespace engine
{
	//- Service responsible for kicking off compilation of a source resource into game/engine-ready format, creating,
	//- deleting and moving asset files, computing final destination location for any given source file,
	//- managing an asset/resource database along with cross-references and dependencies.
	//------------------------------------------------------------------------------------------------------------------------
	class casset_service final : public core::cservice
	{
	public:
		casset_service() = default;
		~casset_service();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		bool compile(stringview_t source_filepath);

		stringview_t compiled_output_path(stringview_t source_filepath) const;

		casset load_asset(stringview_t source_filepath);

	private:

		RTTR_ENABLE(core::cservice);
	};

} //- engine

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(casset_service)
	{
		rttr::cregistrator<casset_service, rttr::detail::as_raw_pointer>("casset_service")
			;
	};

} //- engine