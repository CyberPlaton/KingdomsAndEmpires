include (path.join(SCRIPTS_DIR, "utility.lua"))

function rttr()

	RTTR_SRC = "rttr/src/rttr/"

	project("rttr")
		language ("c++")
		location (path.join(".project", "rttr"))

		kind ("SharedLib")

		files{path.join(RTTR_SRC, "**.cpp"), path.join(RTTR_SRC, "**.h"),
			--  path.join(RTTR_SRC, "destructor.cpp"),
			--  path.join(RTTR_SRC, "enumeration.cpp"),
			--  path.join(RTTR_SRC, "library.cpp"),
			--  path.join(RTTR_SRC, "method.cpp"),
			--  path.join(RTTR_SRC, "parameter_info.cpp"),
			--  path.join(RTTR_SRC, "policy.cpp"),
			--  path.join(RTTR_SRC, "property.cpp"),
			--  path.join(RTTR_SRC, "registration.cpp"),
			--  path.join(RTTR_SRC, "type.cpp"),
			--  path.join(RTTR_SRC, "variant.cpp"),
			--  path.join(RTTR_SRC, "variant_associative_view.cpp"),
			--  path.join(RTTR_SRC, "variant_sequential_view.cpp"),
			--  path.join(RTTR_SRC, "visitor.cpp"),
			--
			--  path.join(RTTR_SRC, "detail/comparison/compare_equal.cpp"),
			--  path.join(RTTR_SRC, "detail/comparison/compare_less.cpp"),
			--  path.join(RTTR_SRC, "detail/misc/standard_types.cpp"),
			--  path.join(RTTR_SRC, "detail/conversion/std_conversion_functions.cpp"),
			--  path.join(RTTR_SRC, "detail/constructor/constructor_wrapper_base.cpp"),
			--  path.join(RTTR_SRC, "detail/destructor/destructor_wrapper_base.cpp"),
			--  path.join(RTTR_SRC, "detail/enumeration/enumeration_helper.cpp"),
			--  path.join(RTTR_SRC, "detail/enumeration/enumeration_wrapper_base.cpp"),
			--  path.join(RTTR_SRC, "detail/library/library_win.cpp"),
			--  path.join(RTTR_SRC, "detail/library/library_unix.cpp"),
			--  path.join(RTTR_SRC, "detail/method/method_wrapper_base.cpp"),
			--  path.join(RTTR_SRC, "detail/parameter_info/parameter_info_wrapper_base.cpp"),
			--  path.join(RTTR_SRC, "detail/property/property_wrapper_base.cpp"),
			--  path.join(RTTR_SRC, "detail/registration/registration_executer.cpp"),
			--  path.join(RTTR_SRC, "detail/registration/registration_state_saver.cpp"),
			--  path.join(RTTR_SRC, "detail/type/type_data.cpp"),
			--  path.join(RTTR_SRC, "detail/type/type_register.cpp"),
			--  path.join(RTTR_SRC, "detail/variant/variant_compare.cpp")
			}

		includedirs {"rttr/src"}
		externalincludedirs {"rttr/src"}
		set_include_path_to_self("rttr")
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))
		set_libs_path()

		-- set additional default defines
		defines{"RTTR_DLL", "RTTR_DLL_EXPORTS"}

		filter{"configurations:debug"}
			symbols "On"
			optimize "Off"

		filter{"configurations:release"}
			symbols "On"
			optimize "Full"
		filter{}
end
rttr()