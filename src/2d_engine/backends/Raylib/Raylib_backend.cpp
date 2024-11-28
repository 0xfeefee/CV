
// Implements:
#include "base.pch.hpp"
#include <2d_engine/backend_hook.hpp>

// Dependencies:
#include <2d_engine/2d_engine.hpp>
#include <2d_engine/2d_engine_api.hpp>
#include <2d_engine/frontend_hook.hpp>
#include <2d_engine/features/systems.hpp>
#include <string>
#include <unordered_map>

// Dependencies (3rd_party):
// Workaround the Raylib name clashes with { windows.h }, does not work with Clang!
namespace rl {
    #include <raylib.h>

    /*
    ## Helpers

        A few helpers to help with conversion of common types.
    */

    static inline rl::Rectangle
    to_rectangle(const cv::Rect rect) {
        return { rect.x, rect.y, rect.z, rect.w };
    }

    static inline rl::Color
    to_color(const cv::Color& color) {
        return { color.x, color.y, color.z, color.w };
    }
}

namespace cv {

    /*
        Raylib backend context.
        - { should_run }: keeps the main loop running.
        - { clear_color }: platform window clear color.
        - { s32x2 }: platform window size.
        - { textures }: raylib source textures, used by subtextures/our API.
    */

    // @todo: replace this...
    constexpr int MAX_TEXTURE_COUNT = 8;
    typedef std::unordered_map<std::string, rl::Texture2D> Texture_Map;

    struct Engine_Context {
        bool         should_run;
        rl::Color    clear_color;
        s32x2        window_size;
        // Texture_Map  loaded_textures;
        Array<rl::Texture2D> textures;

        Engine_Context()
        : clear_color({45, 45, 45, 255}), should_run(true), window_size({0, 0}), textures(MAX_TEXTURE_COUNT) {
        }
    };

    void
    initialize_and_start_backend() {
        Unique<Engine_Config>&  config   = get_context<Engine_Config>();
        Unique<Engine_Context>& context  = get_context<Engine_Context>();
        Unique<Registry>&       registry = get_context<Registry>();

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
            registry->update();
            frontend_step(delta_s);
            registry->get_system<Basic_Velocity_System>().update(delta_s);

            // Render:
            context->should_run = !rl::WindowShouldClose();
            rl::BeginDrawing();
            rl::ClearBackground(context->clear_color); {
                registry->get_system<Rect_Renderer_System>().update();
                registry->get_system<Texture_Renderer_System>().update();
            } rl::EndDrawing();

            // Update window size:
            if (rl::IsWindowResized()) {
                context->window_size = { rl::GetScreenWidth(), rl::GetScreenHeight() };
            }
        }

        // Run user exit code:
        frontend_stop();

        rl::CloseWindow();
    }

    void
    set_clear_color(const u8x4& rgba_color) {
        Unique<Engine_Context>& context = get_context<Engine_Context>();

        context->clear_color = { rgba_color.x, rgba_color.y, rgba_color.z, rgba_color.w };
    }

    bool
    is_key_pressed(Keyboard_Key key) {
        return rl::IsKeyPressed(key);
    }

    void
    draw_rect(const Rect& rect, const Color& fill_color) {
        Unique<Engine_Context>& context = get_context<Engine_Context>();

        rl::DrawRectangle(
           static_cast<int>(rect.x),
           static_cast<int>(rect.y),
           static_cast<int>(rect.z),
           static_cast<int>(rect.w),
           rl::to_color(fill_color)
       );
    }

    Texture
    load_texture(const std::string& texture_file_name) {
        Unique<Engine_Context>& context = get_context<Engine_Context>();

        std::string path = image_path(texture_file_name);
        log_warn("Loading texture: {}", path);

        rl::Texture2D texture = rl::LoadTexture(path.c_str());
        context->textures.set(texture.id, texture);

        return Texture(
            static_cast<int>(texture.id),
            f32x4(0, 0, texture.width, texture.height)
        );
    }

    void
    draw_texture(Texture& texture, const Rect& entity_rect) {
        Unique<Engine_Context>& context     = get_context<Engine_Context>();
        const rl::Texture2D& source_texture = context->textures[texture.id];

        // @todo: we can probably do this somewhere else...
        // If any of the given textures dimensions are 0, inherit dimensions of the source texture.
        if (texture.rect.z == 0.0f || texture.rect.w == 0.0f) {
            texture.rect.z = source_texture.width;
            texture.rect.w = source_texture.height;
        }

        rl::DrawTexturePro(
            source_texture,
            rl::to_rectangle(texture.rect),
            rl::to_rectangle(entity_rect),
            { 0, 0 },
            0,
            { 255, 255, 255, 255 }
        );
    }

} // cv
