include (path.join(SCRIPTS_DIR, "utility.lua"))

function sdl()
	project("sdl")
		language ("c++")
		cppdialect ("c++17")
		location (path.join(".project", "SDL"))

		defines{"SDL_AUDIO_DRIVER_DISK=1", "SDL_AUDIO_DRIVER_DUMMY=1", "SDL_VIDEO_DRIVER_DUMMY=1"}

		-- SDL2 minimal library
		includedirs {
			"SDL/include/",
			"SDL/include/atomic/",
			"SDL/include/audio/",
			"SDL/include/audio/disk/",
			"SDL/include/audio/dummy/",
			"SDL/include/cpuinfo/",
			"SDL/include/dynapi/",
			"SDL/include/events/",
			"SDL/include/file/",
			"SDL/include/haptic/",
			"SDL/include/joystick/",
			"SDL/include/power/",
			"SDL/include/render/",
			"SDL/include/render/software/",
			"SDL/include/stdlib/",
			"SDL/include/thread/",
			"SDL/include/timer/",
			"SDL/include/video/",
			"SDL/include/video/dummy/"
		}

		files {
			"SDL/src/SDL.c",
				"SDL/src/SDL_assert.c",
				"SDL/src/SDL_error.c",
				"SDL/src/SDL_guid.c",
				"SDL/src/SDL_log.c",
				"SDL/src/SDL_dataqueue.c",
				"SDL/src/SDL_hints.c",
				"SDL/src/SDL_list.c",
				"SDL/src/SDL_utils.c",

				"SDL/src/stdlib/SDL_getenv.c",
				"SDL/src/stdlib/SDL_iconv.c",
				"SDL/src/stdlib/SDL_malloc.c",
				"SDL/src/stdlib/SDL_qsort.c",
				"SDL/src/stdlib/SDL_stdlib.c",
				"SDL/src/stdlib/SDL_string.c",
				"SDL/src/stdlib/SDL_strtokr.c",
				"SDL/src/stdlib/SDL_crc16.c",
				"SDL/src/stdlib/SDL_crc32.c",

				"SDL/src/cpuinfo/SDL_cpuinfo.c",

				"SDL/src/thread/SDL_thread.c",

				"SDL/src/atomic/SDL_atomic.c",
				"SDL/src/atomic/SDL_spinlock.c",
				"SDL/src/timer/SDL_timer.c",
				"SDL/src/file/SDL_rwops.c",
				"SDL/src/power/SDL_power.c",

				"SDL/src/audio/SDL_audio.c",
				"SDL/src/audio/SDL_audiocvt.c",
				"SDL/src/audio/SDL_audiodev.c",
				"SDL/src/audio/SDL_audiotypecvt.c",
				"SDL/src/audio/SDL_mixer.c",
				"SDL/src/audio/SDL_wave.c",

				"SDL/src/events/SDL_events.c",
				"SDL/src/events/SDL_quit.c",
				"SDL/src/events/SDL_keyboard.c",
				"SDL/src/events/SDL_mouse.c",
				"SDL/src/events/SDL_windowevents.c",
				"SDL/src/events/SDL_clipboardevents.c",
				"SDL/src/events/SDL_dropevents.c",
				"SDL/src/events/SDL_displayevents.c",
				"SDL/src/events/SDL_gesture.c",
				"SDL/src/events/SDL_touch.c",

				"SDL/src/sensor/SDL_sensor.c",

				"SDL/src/haptic/SDL_haptic.c",

				"SDL/src/hidapi/SDL_hidapi.c",

				"SDL/src/joystick/SDL_gamecontroller.c",
				"SDL/src/joystick/SDL_joystick.c",
				"SDL/src/joystick/controller_type.c",

				"SDL/src/render/SDL_render.c",
				"SDL/src/render/SDL_yuv_sw.c",

				"SDL/src/video/SDL_yuv.c",
				"SDL/src/video/SDL_blit.c",
				"SDL/src/video/SDL_blit_0.c",
				"SDL/src/video/SDL_blit_1.c",
				"SDL/src/video/SDL_blit_A.c",
				"SDL/src/video/SDL_blit_auto.c",
				"SDL/src/video/SDL_blit_copy.c",
				"SDL/src/video/SDL_blit_N.c",
				"SDL/src/video/SDL_blit_slow.c",
				"SDL/src/video/SDL_pixels.c",
				"SDL/src/video/SDL_rect.c",
				"SDL/src/video/SDL_RLEaccel.c",
				"SDL/src/video/SDL_shape.c",
				"SDL/src/video/SDL_stretch.c",
				"SDL/src/video/SDL_surface.c",
				"SDL/src/video/SDL_video.c",
				"SDL/src/video/SDL_vulkan_utils.c",
				"SDL/src/video/SDL_clipboard.c",

				"SDL/src/video/yuv2rgb/yuv_rgb_sse.c",
				"SDL/src/video/yuv2rgb/yuv_rgb_std.c",

				"SDL/src/SDL_blendfillrect.c",
				"SDL/src/SDL_blendline.c",
				"SDL/src/SDL_blendpoint.c",
				"SDL/src/SDL_drawline.c",
				"SDL/src/SDL_drawpoint.c",
				"SDL/src/SDL_render_sw.c",
				"SDL/src/SDL_rotate.c",
				"SDL/src/SDL_triangle.c",
				"SDL/src/SDL_fillrect.c",
				"SDL/src/SDL_bmp.c",

				"SDL/src/SDL_egl.c",
				"SDL/src/SDL_syscond.c",
				"SDL/src/SDL_sysmutex.c",
				"SDL/src/SDL_syssem.c",
				"SDL/src/SDL_systhread.c",
				"SDL/src/SDL_systls.c",
				"SDL/src/SDL_systimer.c",
				"SDL/src/SDL_sysloadso.c",
				"SDL/src/SDL_sysfilesystem.c",
				"SDL/src/SDL_syshaptic.c",
				"SDL/src/SDL_sysjoystick.c",
				"SDL/src/SDL_virtualjoystick.c",
				"SDL/src/SDL_hidapijoystick.c",
				"SDL/src/SDL_hidapi_rumble.c",
				"SDL/src/SDL_hidapi_combined.c",
				"SDL/src/SDL_hidapi_gamecube.c",
				"SDL/src/SDL_hidapi_luna.c",
				"SDL/src/SDL_hidapi_ps3.c",
				"SDL/src/SDL_hidapi_ps4.c",
				"SDL/src/SDL_hidapi_ps5.c",
				"SDL/src/SDL_hidapi_shield.c",
				"SDL/src/SDL_hidapi_stadia.c",
				"SDL/src/SDL_hidapi_switch.c",
				"SDL/src/SDL_hidapi_wii.c",
				"SDL/src/SDL_hidapi_xbox360.c",
				"SDL/src/SDL_hidapi_xbox360w.c",
				"SDL/src/SDL_hidapi_xboxone.c",
				"SDL/src/SDL_hidapi_steam.c",
				"SDL/src/SDL_hidapi_steamdeck.c",
				"SDL/src/SDL_dummyaudio.c",
				"SDL/src/SDL_diskaudio.c",
				"SDL/src/SDL_nullvideo.c",
				"SDL/src/SDL_nullframebuffer.c",
				"SDL/src/SDL_nullevents.c",
				"SDL/src/SDL_dummysensor.c",
				"SDL/src/SDL_locale.c",
				"SDL/src/SDL_syslocale.c",
				"SDL/src/SDL_url.c",
				"SDL/src/SDL_sysurl.c",
				"SDL/src/SDL_winmm.c",
				"SDL/src/SDL_directsound.c",
				"SDL/src/SDL_wasapi.c",
				"SDL/src/SDL_wasapi_win32.c",

				"SDL/src/core/windows/SDL_hid.c",
				"SDL/src/core/windows/SDL_immdevice.c",
				"SDL/src/core/windows/SDL_windows.c",
				"SDL/src/core/windows/SDL_xinput.c",

				"SDL/src/SDL_dinputhaptic.c",
				"SDL/src/SDL_windowshaptic.c",
				"SDL/src/SDL_xinputhaptic.c",
				"SDL/src/SDL_dinputjoystick.c",
				"SDL/src/SDL_rawinputjoystick.c",
				"SDL/src/SDL_windowsjoystick.c",
				"SDL/src/SDL_windows_gaming_input.c",
				"SDL/src/SDL_xinputjoystick.c",
				"SDL/src/SDL_syspower.c",
				"SDL/src/SDL_d3dmath.c",
				"SDL/src/SDL_render_d3d.c",
				"SDL/src/SDL_shaders_d3d.c",
				"SDL/src/SDL_render_d3d11.c",
				"SDL/src/SDL_shaders_d3d11.c",
				"SDL/src/SDL_render_d3d12.c",
				"SDL/src/SDL_shaders_d3d12.c",
				"SDL/src/SDL_render_gl.c",
				"SDL/src/SDL_shaders_gl.c",
				"SDL/src/SDL_render_gles2.c",
				"SDL/src/SDL_shaders_gles2.c",
				"SDL/src/SDL_windowssensor.c",
				"SDL/src/SDL_syscond_cv.c",
				"SDL/src/SDL_windowsclipboard.c",
				"SDL/src/SDL_windowsevents.c",
				"SDL/src/SDL_windowsframebuffer.c",
				"SDL/src/SDL_windowskeyboard.c",
				"SDL/src/SDL_windowsmessagebox.c",
				"SDL/src/SDL_windowsmodes.c",
				"SDL/src/SDL_windowsmouse.c",
				"SDL/src/SDL_windowsopengl.c",
				"SDL/src/SDL_windowsopengles.c",
				"SDL/src/SDL_windowsshape.c",
				"SDL/src/SDL_windowsvideo.c",
				"SDL/src/SDL_windowsvulkan.c",
				"SDL/src/SDL_windowswindow.c",
				"SDL/src/SDL_steam_virtual_gamepad.c",

				"SDL/src/dynapi/SDL_dynapi.c",

				"SDL/include/SDL_config_windows.h",
				"SDL/src/thread/generic/SDL_syscond.c",
				"SDL/src/thread/generic/SDL_sysmutex_c.h",
		}

		-- SDL2 platform specific
		-- macosx and linux require a static library, windows wants a
		-- dynamically linked SDL2
		if PLATFORM == "windows" then
			kind ("SharedLib")
		else
			kind ("StaticLib")
		end

		if PLATFORM == "windows" then
			defines{"_WINDOWS",
					"SDL_LOADSO_WINDOWS=1",
					"SDL_THREAD_WINDOWS=1",
					"SDL_TIMER_WINDOWS=1",
					"SDL_VIDEO_DRIVER_WINDOWS=1",
					"SDL_POWER_WINDOWS=1",
					"SDL_AUDIO_DRIVER_WINMM=1",
					"SDL_FILESYSTEM_WINDOWS=1",
					"SDL_AUDIO_DRIVER_DSOUND=1",
					"SDL_AUDIO_DRIVER_XAUDIO2=1",
					"SDL_JOYSTICK_DINPUT=1",
					"SDL_HAPTIC_DINPUT=1",
					"SDL_VIDEO_RENDER_D3D=1"}

			includedirs {
				"SDL/include/audio/winmm/",
				"SDL/include/core/windows/",
				"SDL/include/libm/",
				"SDL/include/loadso/windows/",
				"SDL/include/power/windows/",
				"SDL/include/thread/windows/",
				"SDL/include/timer/windows/",
				"SDL/include/video/windows/",
				"SDL/include/filesystem/windows/",
				"SDL/include/audio/directsound/",
				"SDL/include/audio/xaudio2/",
				"SDL/include/render/direct3d/",
				-- these two depend on Xinput
				"SDL/include/haptic/windows/",
				"SDL/include/joystick/windows/",
			}

			files {
				"SDL/src/SDL.c",
				"SDL/src/SDL_assert.c",
				"SDL/src/SDL_error.c",
				"SDL/src/SDL_guid.c",
				"SDL/src/SDL_log.c",
				"SDL/src/SDL_dataqueue.c",
				"SDL/src/SDL_hints.c",
				"SDL/src/SDL_list.c",
				"SDL/src/SDL_utils.c",

				"SDL/src/SDL_getenv.c",
				"SDL/src/SDL_iconv.c",
				"SDL/src/SDL_malloc.c",
				"SDL/src/SDL_qsort.c",
				"SDL/src/SDL_stdlib.c",
				"SDL/src/SDL_string.c",
				"SDL/src/SDL_strtokr.c",
				"SDL/src/SDL_crc16.c",
				"SDL/src/SDL_crc32.c",

				"SDL/src/cpuinfo/SDL_cpuinfo.c",

				"SDL/src/thread/SDL_thread.c",

				"SDL/src/SDL_atomic.c",
				"SDL/src/SDL_spinlock.c",
				"SDL/src/SDL_timer.c",
				"SDL/src/SDL_rwops.c",
				"SDL/src/SDL_power.c",

				"SDL/src/audio/SDL_audio.c",
				"SDL/src/audio/SDL_audiocvt.c",
				"SDL/src/audio/SDL_audiodev.c",
				"SDL/src/audio/SDL_audiotypecvt.c",
				"SDL/src/audio/SDL_mixer.c",
				"SDL/src/audio/SDL_wave.c",

				"SDL/src/events/SDL_events.c",
				"SDL/src/events/SDL_quit.c",
				"SDL/src/events/SDL_keyboard.c",
				"SDL/src/events/SDL_mouse.c",
				"SDL/src/events/SDL_windowevents.c",
				"SDL/src/events/SDL_clipboardevents.c",
				"SDL/src/events/SDL_dropevents.c",
				"SDL/src/events/SDL_displayevents.c",
				"SDL/src/events/SDL_gesture.c",
				"SDL/src/events/SDL_touch.c",

				"SDL/src/sensor/SDL_sensor.c",

				"SDL/src/haptic/SDL_haptic.c",

				"SDL/src/hidapi/SDL_hidapi.c",

				"SDL/src/joystick/SDL_gamecontroller.c",
				"SDL/src/joystick/SDL_joystick.c",
				"SDL/src/joystick/controller_type.c",

				"SDL/src/render/SDL_render.c",
				"SDL/src/render/SDL_yuv_sw.c",

				"SDL/src/yuv_rgb_sse.c",
				"SDL/src/yuv_rgb_std.c",
				"SDL/src/SDL_yuv.c",

				"SDL/src/SDL_blendfillrect.c",
				"SDL/src/SDL_blendline.c",
				"SDL/src/SDL_blendpoint.c",
				"SDL/src/SDL_drawline.c",
				"SDL/src/SDL_drawpoint.c",
				"SDL/src/SDL_render_sw.c",
				"SDL/src/SDL_rotate.c",
				"SDL/src/SDL_triangle.c",
				"SDL/src/SDL_blit.c",
				"SDL/src/SDL_blit_0.c",
				"SDL/src/SDL_blit_1.c",
				"SDL/src/SDL_blit_A.c",
				"SDL/src/SDL_blit_auto.c",
				"SDL/src/SDL_blit_copy.c",
				"SDL/src/SDL_blit_N.c",
				"SDL/src/SDL_blit_slow.c",
				"SDL/src/SDL_fillrect.c",
				"SDL/src/SDL_bmp.c",
				"SDL/src/SDL_pixels.c",
				"SDL/src/SDL_rect.c",
				"SDL/src/SDL_RLEaccel.c",
				"SDL/src/SDL_shape.c",
				"SDL/src/SDL_stretch.c",
				"SDL/src/SDL_surface.c",
				"SDL/src/SDL_video.c",
				"SDL/src/SDL_clipboard.c",
				"SDL/src/SDL_vulkan_utils.c",
				"SDL/src/SDL_egl.c",
				"SDL/src/SDL_syscond.c",
				"SDL/src/SDL_sysmutex.c",
				"SDL/src/SDL_syssem.c",
				"SDL/src/SDL_systhread.c",
				"SDL/src/SDL_systls.c",
				"SDL/src/SDL_systimer.c",
				"SDL/src/SDL_sysloadso.c",
				"SDL/src/SDL_sysfilesystem.c",
				"SDL/src/SDL_syshaptic.c",
				"SDL/src/SDL_sysjoystick.c",
				"SDL/src/SDL_virtualjoystick.c",
				"SDL/src/SDL_hidapijoystick.c",
				"SDL/src/SDL_hidapi_rumble.c",
				"SDL/src/SDL_hidapi_combined.c",
				"SDL/src/SDL_hidapi_gamecube.c",
				"SDL/src/SDL_hidapi_luna.c",
				"SDL/src/SDL_hidapi_ps3.c",
				"SDL/src/SDL_hidapi_ps4.c",
				"SDL/src/SDL_hidapi_ps5.c",
				"SDL/src/SDL_hidapi_shield.c",
				"SDL/src/SDL_hidapi_stadia.c",
				"SDL/src/SDL_hidapi_switch.c",
				"SDL/src/SDL_hidapi_wii.c",
				"SDL/src/SDL_hidapi_xbox360.c",
				"SDL/src/SDL_hidapi_xbox360w.c",
				"SDL/src/SDL_hidapi_xboxone.c",
				"SDL/src/SDL_hidapi_steam.c",
				"SDL/src/SDL_hidapi_steamdeck.c",
				"SDL/src/SDL_dummyaudio.c",
				"SDL/src/SDL_diskaudio.c",
				"SDL/src/SDL_nullvideo.c",
				"SDL/src/SDL_nullframebuffer.c",
				"SDL/src/SDL_nullevents.c",
				"SDL/src/SDL_dummysensor.c",
				"SDL/src/SDL_locale.c",
				"SDL/src/SDL_syslocale.c",
				"SDL/src/SDL_url.c",
				"SDL/src/SDL_sysurl.c",
				"SDL/src/SDL_winmm.c",
				"SDL/src/SDL_directsound.c",
				"SDL/src/SDL_wasapi.c",
				"SDL/src/SDL_wasapi_win32.c",

				"SDL/src/core/windows/SDL_hid.c",
				"SDL/src/core/windows/SDL_immdevice.c",
				"SDL/src/core/windows/SDL_windows.c",
				"SDL/src/core/windows/SDL_xinput.c",

				"SDL/src/SDL_dinputhaptic.c",
				"SDL/src/SDL_windowshaptic.c",
				"SDL/src/SDL_xinputhaptic.c",
				"SDL/src/SDL_dinputjoystick.c",
				"SDL/src/SDL_rawinputjoystick.c",
				"SDL/src/SDL_windowsjoystick.c",
				"SDL/src/SDL_windows_gaming_input.c",
				"SDL/src/SDL_xinputjoystick.c",
				"SDL/src/SDL_syspower.c",
				"SDL/src/SDL_d3dmath.c",
				"SDL/src/SDL_render_d3d.c",
				"SDL/src/SDL_shaders_d3d.c",
				"SDL/src/SDL_render_d3d11.c",
				"SDL/src/SDL_shaders_d3d11.c",
				"SDL/src/SDL_render_d3d12.c",
				"SDL/src/SDL_shaders_d3d12.c",
				"SDL/src/SDL_render_gl.c",
				"SDL/src/SDL_shaders_gl.c",
				"SDL/src/SDL_render_gles2.c",
				"SDL/src/SDL_shaders_gles2.c",
				"SDL/src/SDL_windowssensor.c",
				"SDL/src/SDL_syscond_cv.c",
				"SDL/src/SDL_windowsclipboard.c",
				"SDL/src/SDL_windowsevents.c",
				"SDL/src/SDL_windowsframebuffer.c",
				"SDL/src/SDL_windowskeyboard.c",
				"SDL/src/SDL_windowsmessagebox.c",
				"SDL/src/SDL_windowsmodes.c",
				"SDL/src/SDL_windowsmouse.c",
				"SDL/src/SDL_windowsopengl.c",
				"SDL/src/SDL_windowsopengles.c",
				"SDL/src/SDL_windowsshape.c",
				"SDL/src/SDL_windowsvideo.c",
				"SDL/src/SDL_windowsvulkan.c",
				"SDL/src/SDL_windowswindow.c",
				"SDL/src/SDL_steam_virtual_gamepad.c",

				"SDL/src/dynapi/SDL_dynapi.c",

				"SDL/include/SDL_config_windows.h",
				"SDL/src/thread/generic/SDL_syscond.c",
				"SDL/src/thread/generic/SDL_sysmutex_c.h",
			}

			links {"setupapi", "winmm", "imm32", "version", "oleaut32"}
		elseif PLATFORM == "linux" then
		elseif PLATFORM == "macosx" then
			files{"SDL/include/SDL_config_macosx.h"}
		end

		set_basic_defines()
		targetdir(path.join(VENDOR_DIR, OUTDIR))
		objdir(path.join(VENDOR_DIR, OUTDIR, ".obj"))

		filter{"configurations:debug"}
			symbols "On"
			optimize "Off"

		filter{"configurations:release"}
			symbols "On"
			optimize "Full"
		filter{}
end
sdl()