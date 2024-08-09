include (path.join(SCRIPTS_DIR, "utility.lua"))

function asio()
	add_target_library("asio",
					{},
					{"ASIO_STANDALONE", "ASIO_NO_EXCEPTIONS", "ASIO_NO_DEPRECATED"},
					{},
					{},
					true,
					"c++",
					"include/asio/asio/include")
end
asio()
