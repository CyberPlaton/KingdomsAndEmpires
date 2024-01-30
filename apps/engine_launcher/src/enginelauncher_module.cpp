#include "enginelauncher_module.hpp"
#include <iostream>

void logging_function(const char* tag, uint32_t level, uint32_t item_id, const char* message, uint32_t line, const char* filename, void* data)
{
	logging::log_debug(fmt::format("[SOKOL] {}", message).data());
}

// Called on every frame of the application.
static void frame(void) {
	// Get current window size.
	int width = sapp_width(), height = sapp_height();
	float ratio = width / (float)height;

	// Begin recording draw commands for a frame buffer of size (width, height).
	sgp_begin(width, height);
	// Set frame buffer drawing region to (0,0,width,height).
	sgp_viewport(0, 0, width, height);
	// Set drawing coordinate space to (left=-ratio, right=ratio, top=1, bottom=-1).
	sgp_project(-ratio, ratio, 1.0f, -1.0f);

	// Clear the frame buffer.
	sgp_set_color(0.1f, 0.1f, 0.1f, 1.0f);
	sgp_clear();

	// Draw an animated rectangle that rotates and changes its colors.
	float time = sapp_frame_count() * sapp_frame_duration();
	float r = sinf(time) * 0.5 + 0.5, g = cosf(time) * 0.5 + 0.5;
	sgp_set_color(r, g, 0.3f, 1.0f);
	sgp_rotate_at(time, 0.0f, 0.0f);
	sgp_draw_filled_rect(-0.5f, -0.5f, 1.0f, 1.0f);

	// Begin a render pass.
	sg_pass_action pass_action = { 0 };
	sg_begin_default_pass(&pass_action, width, height);
	// Dispatch all draw commands to Sokol GFX.
	sgp_flush();
	// Finish a draw command queue, clearing it.
	sgp_end();
	// End render pass.
	sg_end_pass();
	// Commit Sokol render.
	sg_commit();

	//ecs::cworld_manager::instance().current().progress(time);
}

// Called when the application is initializing.
void init(void)
{
	logging::log_debug(fmt::format("Initializing SOKOL").data());

	// Initialize Sokol GFX.
	sg_desc sgdesc{ 0 }; 
	sgdesc.context = sapp_sgcontext();
	sgdesc.logger.func = logging_function;

	sg_setup(&sgdesc);
	if (!sg_isvalid()) {
		fprintf(stderr, "Failed to create Sokol GFX context!\n");
		exit(-1);
	}

	// Initialize Sokol GP, adjust the size of command buffers for your own use.
	sgp_desc sgpdesc = { 0 };
	sgp_setup(&sgpdesc);
	if (!sgp_is_valid()) {
		fprintf(stderr, "Failed to create Sokol GP context: %s\n", sgp_get_error_message(sgp_get_last_error()));
		exit(-1);
	}
}

// Called when the application is shutting down.
void cleanup(void)
{
	logging::log_debug(fmt::format("Shutting down SOKOL").data());
	// Cleanup Sokol GP and Sokol GFX resources.
	sgp_shutdown();
	sg_shutdown();
}


struct ssome_data : io::iserializable
{
	void save(TArchiveOut& archive) const override
	{
		archive(m_uuid, m_data);
	}

	void load(TArchiveIn& archive) override
	{
		archive(m_uuid, m_data);
	}

	vector_t<unsigned> m_data;
	string_t m_uuid;
};


#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	AllocConsole();

	core::cfile file(core::cfilesystem::construct("somepath", ".json"), core::file_read_write_mode_write
		| core::file_read_write_mode_text
		| core::file_read_write_mode_cereal);

	core::cuuid uuid;
	ssome_data datastructure, datastructure2;
	datastructure.m_data.emplace_back(1);
	datastructure.m_uuid = uuid.c_str();

	file.write_sync_cereal(datastructure);

	core::cfile file2(core::cfilesystem::construct("somepath", ".json"), core::file_read_write_mode_read
		| core::file_read_write_mode_text
		| core::file_read_write_mode_cereal);

	file2.read_sync_cereal(datastructure2);


	logging::init();
	logging::log_debug(fmt::format("Starting on WinMain()").data());
	logging::log_trace("Log log_trace");
	logging::log_debug("Log log_debug");
	logging::log_info("Log log_info");
	logging::log_warn("Log log_warn");
	logging::log_error("Log log_error");
	logging::log_critical("Log log_critical");


	//- Important! Create a new world
	//ecs::cworld_manager::instance().create("My World");

	//- Create module manager
	//ecs::cmodule_manager module_manager;

	//- Load modules: Should be done by module manager
	//module_example::cmy_second_module my_module(ecs::cworld_manager::instance().current());


	sapp_desc desc{ 0 };
	desc.width = 720;
	desc.height = 648;
	desc.sample_count = 1;
	desc.swap_interval = 1;
	desc.high_dpi = false;
	desc.fullscreen = false;
	desc.alpha = true;
	desc.window_title = "Rectangle (Sokol GP)";
	desc.clipboard_size = 0;
	desc.max_dropped_files = 0;
	desc.max_dropped_file_path_length = 0;

	desc.init_cb = init;
	desc.frame_cb = frame;
	desc.cleanup_cb = cleanup;
	desc.logger.func = logging_function;

	sapp_run(&desc);

	return 0;
}
#else
int main(int argc, char* argv[])
{
	logging::init();
	logging::log_debug(fmt::format("Starting on main()"));
	logging::log_trace("Log log_trace");
	logging::log_debug("Log log_debug");
	logging::log_info("Log log_info");
	logging::log_warn("Log log_warn");
	logging::log_error("Log log_error");
	logging::log_critical("Log log_critical");

	sapp_desc desc{ 0 };
	desc.width = 720;
	desc.height = 648;
	desc.sample_count = 1;
	desc.swap_interval = 1;
	desc.high_dpi = false;
	desc.fullscreen = false;
	desc.alpha = true;
	desc.window_title = "Rectangle (Sokol GP)";
	desc.clipboard_size = 0;
	desc.max_dropped_files = 0;
	desc.max_dropped_file_path_length = 0;

	desc.init_cb = NULL;
	desc.frame_cb = frame;
	desc.cleanup_cb = NULL;
	desc.logger.func = logging_function;

	sapp_run(&desc);

	return 0;
}
#endif