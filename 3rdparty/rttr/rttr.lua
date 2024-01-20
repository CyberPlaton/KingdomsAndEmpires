include (path.join(SCRIPTS_DIR, "utility.lua"))

function rttr()
	project("rttr")
		language ("c++")
		location (path.join(".project", "rttr"))

		kind ("StaticLib")

		files{"src/rttr_module.hpp", "src/rttr_module.cpp",
              "src/constructor.cpp",
              "src/destructor.cpp",
              "src/enumeration.cpp",
              "src/library.cpp",
              "src/method.cpp",
              "src/parameter_info.cpp",
              "src/policy.cpp",
              "src/property.cpp",
              "src/registration.cpp",
              "src/type.cpp",
              "src/variant.cpp",
              "src/variant_associative_view.cpp",
              "src/variant_sequential_view.cpp",
              "src/visitor.cpp",
              "src/detail/comparison/compare_equal.cpp",
              "src/detail/comparison/compare_less.cpp",
              "src/detail/misc/standard_types.cpp",
              "src/detail/conversion/std_conversion_functions.cpp",
              "src/detail/constructor/constructor_wrapper_base.cpp",
              "src/detail/destructor/destructor_wrapper_base.cpp",
              "src/detail/enumeration/enumeration_helper.cpp",
              "src/detail/enumeration/enumeration_wrapper_base.cpp",
              "src/detail/library/library_win.cpp",
              "src/detail/library/library_unix.cpp",
              "src/detail/method/method_wrapper_base.cpp",
              "src/detail/parameter_info/parameter_info_wrapper_base.cpp",
              "src/detail/property/property_wrapper_base.cpp",
              "src/detail/registration/registration_executer.cpp",
              "src/detail/registration/registration_state_saver.cpp",
              "src/detail/type/type_data.cpp",
              "src/detail/type/type_register.cpp",
              "src/detail/variant/variant_compare.cpp",}

		set_include_path_to_self("rttr")
		set_include_path_to_engine()
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()

		-- set additional default defines
		defines{""}

		filter{"configurations:debug"}
			symbols "On"
			optimize "Off"

		filter{"configurations:release"}
			symbols "On"
			optimize "Full"
		filter{}
end
rttr()