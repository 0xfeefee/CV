
// Implements:
#include <2d_engine/backend_hook.hpp>

// Dependencies:
#include <2d_engine/2d_engine.hpp>
#include <2d_engine/backend_api.hpp>
#include <2d_engine/frontend_hook.hpp>

// Dependencies (3rd_party):
namespace rl {
    #include <raylib.h>
}


namespace cv {

    /*
        Raylib backend context.
    */
    struct Engine_Context {
        rl::Color clear_color;
        s32x2 window_size;
        bool should_run;

        Engine_Context()
        : clear_color({45, 45, 45, 255}), should_run(true), window_size({0, 0}) {
        }
    };

    void
    initialize_and_start_backend() {
        Unique<Engine_Config>&  config   = get_context<Engine_Config>();
        Unique<Engine_Context>& context  = get_context<Engine_Context>();

        // Raylib should only log errors:
        rl::SetTraceLogLevel(rl::LOG_ERROR);

        // Set initial window size:
        context->window_size = { config->window_width, config->window_height };

        // Translate Engine_Flags to raylib Config_Flags:
        int raylib_flags = 0;
        if (config->flags & Engine_Flags_Borderless) {
            raylib_flags |= rl::FLAG_WINDOW_UNDECORATED;
        }

        if (config->flags & Engine_Flags_Fullscreen) {
            raylib_flags |= rl::FLAG_FULLSCREEN_MODE;
        }

        if (config->flags & Engine_Flags_Vsync) {
            raylib_flags |= rl::FLAG_VSYNC_HINT;
        }

        rl::SetConfigFlags(raylib_flags);

        // Create a window:
        rl::InitWindow(config->window_width, config->window_height, config->window_title.c_str());

        /*
            When { VSYNC } is enabled the framerate will set to the monitor refresh rate, we can only get the
            monitor after window was created. We always assume it's the primary monitor - at index 0.
        */
        if (config->flags & Engine_Flags_Vsync) {
            s16 monitor_refresh_rate = static_cast<s16>(rl::GetMonitorRefreshRate(0));
            if (monitor_refresh_rate > 0 && config->desired_framerate != monitor_refresh_rate) {
                log_warn(
                    "VSYNC enabled, refresh rate override: {} -> {}",
                    config->desired_framerate,
                    monitor_refresh_rate
                );
                config->desired_framerate = monitor_refresh_rate;
            }
        }

        // Main loop:
        const f64 S_PER_FRAME = 1.0/config->desired_framerate;
        f64 last_elapsed_s    = rl::GetTime();

        // Run user code to init/start the game:
        frontend_start();

        while (context->should_run) {
            // Wait out the extra time
            f64 s_to_wait = S_PER_FRAME - (rl::GetTime() - last_elapsed_s);
            if (s_to_wait > 0 && s_to_wait <= S_PER_FRAME) {
                rl::WaitTime(s_to_wait);
            }

            // Calculate the new delta time:
            f64 current_elapsed_s = rl::GetTime();
            f64 delta_s           = current_elapsed_s - last_elapsed_s;
            last_elapsed_s        = current_elapsed_s;

            // Run user frame code:
            frontend_step(delta_s);

            // Render:
            context->should_run = !rl::WindowShouldClose();
            rl::BeginDrawing();
            rl::ClearBackground(context->clear_color);
            rl::EndDrawing();

            // Update window size:
            if (rl::IsWindowResized()) {
                context->window_size = { rl::GetScreenWidth(), rl::GetScreenHeight() };
            }
        }

        // Run user exit code:
        frontend_stop();

        rl::CloseWindow();
    }

} // cv
