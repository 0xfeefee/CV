
// Implements:
#include <engine/core/backend_hook.hpp>

// Dependencies:
#include <engine/core/engine.hpp>
#include <engine/core/frontend_hook.hpp>
#include <features/features.hpp>

// Dependencies (3rd_party):
// Workaround the Raylib name clashes with { windows.h }, does not work with Clang!
namespace rl {
    #if PROJECT_PLATFORM_WIN64
        // Undef the macros which clash with raylib functions:
        #undef DrawText
        #undef DrawTextEx
        #undef LoadImage
    #endif

    #define GLSL_VERSION 330
    #include <raylib.h>
    #include <rlgl.h>

    /*
    ## Helpers

        A few helpers to help with conversion of common types.
    */

    static inline rl::Rectangle
    to_rectangle(const jbx::Rect rect) {
        return { rect.x, rect.y, rect.z, rect.w };
    }

    static inline rl::Color
    to_color(const jbx::Color& color) {
        return { color.x, color.y, color.z, color.w };
    }

    Vector3 vec3_zero(void) {
        return { 0.0f, 0.0f, 0.0f };
    }
} // rl

namespace jbx {

    /*
        Raylib backend context.
        - { should_run }: keeps the main loop running.
        - { clear_color }: platform window clear color.
        - { s32x2 }: platform window size.
        - { textures }: raylib source textures, used by subtextures/our API.
    */

    // @todo: replace this...
    constexpr int MAX_TEXTURE_COUNT = 16;
    constexpr int MAX_SOUND_COUNT   = 16;
    constexpr int MAX_FONT_COUNT    = 16;

    struct Engine_Context {
        bool                 should_run;
        rl::Color            clear_color;
        s32x2                window_size;
        Array<rl::Texture2D> textures;
        Array<rl::Sound>     sounds;
        Array<rl::Font>      fonts;

        // Temporary:
        std::atomic<bool>    images_loaded;
        Array<rl::Image>     images;

        Engine_Context()
        : clear_color({45, 45, 45, 255}),
          should_run(true),
          window_size({0, 0}),
          textures(MAX_TEXTURE_COUNT),
          sounds(MAX_SOUND_COUNT),
          fonts(MAX_FONT_COUNT),
          images(2) {
        }
    };

    static cstr_t
    invert_fs = R"(
        #version 330

        in vec2 fragTexCoord;
        out vec4 fragColor;

        uniform sampler2D texture0;

        void main() {
            vec4 texColor = texture(texture0, fragTexCoord);
            fragColor = vec4(1.0 - texColor.rgb, texColor.a);
        }
    )";


    static void
    resource_thread_fn() {
        Sleep(1024);
        Unique<Engine_Context>& context = get_context<Engine_Context>();
        // Adding tilemap just to test it all out, even though it will be loaded on it's own
        // later.
        for (const std::string& image_name: {"colormap", "Crt_UV"}) {
            // We just assume that only the resource thread will ever write to this array:
            context->images.add(rl::LoadImage(image_path(image_name).c_str()));
        }

        context->images_loaded = true;
    }


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
        if (config->flags & Engine_Flags_No_Decoration) {
            raylib_flags |= rl::FLAG_WINDOW_UNDECORATED;
        }

        if (config->flags & Engine_Flags_Fullscreen) {
            raylib_flags |= rl::FLAG_FULLSCREEN_MODE;
        }

        if (config->flags & Engine_Flags_Vsync) {
            raylib_flags |= rl::FLAG_VSYNC_HINT;
        }

        raylib_flags |= rl::FLAG_MSAA_4X_HINT;

        rl::SetConfigFlags(raylib_flags);

        // Create a window:
            rl::InitWindow(config->window_width, config->window_height, config->window_title.c_str());
        rl::InitAudioDevice();

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


        /*
            Setup the render texture, 3D camera and a simple plane primitive.
            - Camera faces the plane, we use the standard 45 degree FOV.
            - Plane is 10x10 though perhaps we have to change the size depending on the 2d engine window
            size.

            We will render the output of the 2D game engine onto the render texture, then we will render
            this onto another offscreen texture to apply post processing, and this texture will be used as
            an diffuse map for the plane in 3D space.

            { frame_texture } is the 2D game scene, { model_texture } is { frame_texture } with post-processing
            applied (invert colors in this case). Since rendering to a texture effectively flips it,
            rendering it again to apply post-processing flips it back to normal.

            When updating the camera we use the { FREE_CAMERA } mode which allows us to move it with
            { W, A, S, D }

            Throwaway code!

            At the moment I'm just figuring out what sort of data type we will use for model loading, or how
            these will be rendered, while composing the final scene in Blender.
        */
        rl::RenderTexture2D frame_texture = rl::LoadRenderTexture(config->window_width, config->window_height);
        rl::RenderTexture2D model_texture = rl::LoadRenderTexture(config->window_width, config->window_height);

        rl::Shader shader_2d    = rl::LoadShaderFromMemory(nullptr, invert_fs);

        rl::Camera3D camera = { 0 };
        camera.position     = { 0.2f, 5.0f, 5.0f };
        camera.target       = { 0.0f, 0.0f, 0.0f };
        camera.up           = { 0.0f, 1.0f, 0.0f };
        camera.fovy         = 45.0f;
        camera.projection   = rl::CAMERA_PERSPECTIVE;

        std::thread resource_thread(resource_thread_fn);
        resource_thread.detach();

        rl::Model plane = rl::LoadModelFromMesh(rl::GenMeshPlane(8.0f, 6.0f, 1, 1));
        plane.materials[0].maps[rl::MATERIAL_MAP_DIFFUSE].texture = model_texture.texture;

        rl::Model donut = rl::LoadModel(model_path_glb("donut-sprinkles").c_str());
        rl::Model crt_monitor = rl::LoadModel(model_path_glb("crt2").c_str());

        // Main loop:
        const f64 S_PER_FRAME = 1.0/config->desired_framerate;
        f64 last_elapsed_s    = rl::GetTime();

        // Run user code to init/start the game:
        frontend_start();
        rl::DisableCursor();

        float x = 0.0f, y = 0.0f, z = 0.0f;

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

            if (rl::IsKeyPressed(rl::KEY_SPACE)) {
                x += 0.1f;
            }
            if (rl::IsKeyPressed(rl::KEY_LEFT_SHIFT)) {
                y += 0.1f;
            }

            // Update the 3D camera:
            rl::UpdateCamera(&camera, rl::CAMERA_FREE);

            // @temp: Check for any resources that need to be loaded in:
            {
                if (context->images_loaded) {
                    rl::Texture2D colormap = rl::LoadTextureFromImage(context->images[0]);
                    for (int i = 0; i < donut.materialCount; i++) {
                        rl::SetMaterialTexture(&donut.materials[i], rl::MATERIAL_MAP_DIFFUSE, colormap);
                    }

                    colormap = rl::LoadTextureFromImage(context->images[1]);
                    for (int i = 0; i < crt_monitor.materialCount; i++) {
                        rl::SetMaterialTexture(&crt_monitor.materials[i], rl::MATERIAL_MAP_DIFFUSE, colormap);
                    }

                    context->images_loaded = false;
                }
            }

            // Render:
            context->should_run = !rl::WindowShouldClose();

            // Render the 2D engine scene onto the frame texture:
            {
                rl::BeginTextureMode(frame_texture); {
                    rl::ClearBackground({0,0,0,0});
                    registry->get_system<Rect_Renderer_System>().update(delta_s);
                    registry->get_system<Texture_Renderer_System>().update(delta_s);
                    registry->get_system<Text_Renderer_System>().update(delta_s);
                } rl::EndTextureMode();

                // Apply post-processing:
                rl::BeginTextureMode(model_texture); {
                    rl::ClearBackground({0,0,0,50});
                    rl::BeginShaderMode(shader_2d);
                    rl::DrawTexture(frame_texture.texture, 0, 0, {255,255,255,255});
                    rl::EndShaderMode();
                } rl::EndTextureMode();

                // Render the 3D scene:
                rl::BeginDrawing();
                rl::ClearBackground({0,0,0,255}); {
                    rl::BeginMode3D(camera);
                    rl::rlDisableBackfaceCulling();
                    rl::DrawModel(plane, { 0.0f, 3.0f, 0.0f }, 1.25f, rl::WHITE);
                    rl::rlEnableBackfaceCulling();
                    rl::DrawModel(donut, { 0.0f, 4.0f, 5.0f }, 5.0f, rl::WHITE);
                    rl::DrawModel(crt_monitor, { 0.0f, 0.0f, 8.0f }, 0.25f, rl::WHITE);
                    rl::EndMode3D();
                } rl::EndDrawing();
            }

            // Update window size:
            if (rl::IsWindowResized()) {
                context->window_size = { rl::GetScreenWidth(), rl::GetScreenHeight() };
            }
        }

        // Run user exit code:
        frontend_stop();

        rl::CloseAudioDevice();
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

    Sound
    load_sound(const std::string& sound_file_name, f32 volume, f32 pitch) {
        Unique<Engine_Context>& context = get_context<Engine_Context>();

        std::string path = sound_path(sound_file_name);

        rl::Sound sound = rl::LoadSound(path.c_str());
        int sound_id = context->sounds.get_count();
        context->sounds.add(sound);

        return Sound(
            sound_id,
            volume,
            pitch
        );
    }

    void
    play_sound(Sound& sound) {
        Unique<Engine_Context>& context = get_context<Engine_Context>();
        rl::Sound& source_sound         = context->sounds[sound.id];

        // @todo: avoid repeating this too much!
        // rl::SetSoundVolume(source_sound, sound.volume);
        // rl::SetSoundPitch(source_sound, sound.pitch);

        rl::PlaySound(source_sound);
    }

    Font
    load_font(const std::string& font_file_name, int font_size) {
        Unique<Engine_Context>& context = get_context<Engine_Context>();

        std::string font_file_path = font_path(font_file_name);
        log_warn("Loading font: {} with size: {}", font_file_path, font_size);

        rl::Font source_font = rl::LoadFontEx(font_file_path.c_str(), font_size, nullptr, 0);
        int font_id = context->fonts.get_count();
        context->fonts.add(source_font);

        return Font(font_id);
    }

    void
    draw_text(const Text& text, f32x2 position) {
        constexpr int default_spacing     = 2;
        constexpr rl::Color default_color = { 255,255,255,255 };

        Unique<Engine_Context>& context = get_context<Engine_Context>();
        const rl::Font& source_font     = context->fonts[text.font.id];

        rl::DrawTextEx(
            source_font,
            text.data.c_str(),
            { position.x, position.y },
            source_font.baseSize,
            default_spacing,
            rl::to_color(text.color)
        );
    }

} // jbx
