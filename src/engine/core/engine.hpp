
#pragma once
//@temporary
namespace jbx {

    /*
        Flags used to configure the platform windows
        - { Window_Flags_None }:       Nothing.
        - { Window_Flags_Vsync }:      Enable VSYNC on the platform window.
        - { Window_Flags_Fullscreen }: Set the platform window video mode to fullscreen.
        - { Window_Flags_Borderless }: Remove all decorations from the platform window.
    */
    typedef u8 Engine_Flags;
    enum Engine_Flags_ : u8 {
        Engine_Flags_None          = 0,        // 0000 0000
        Engine_Flags_Vsync         = 1 << 0,   // 0000 0001
        Engine_Flags_Fullscreen    = 1 << 1,   // 0000 0010
        Engine_Flags_No_Decoration = 1 << 2    // 0000 0100
    };

    /*
        Configure the engine, when unspecified or invalid the configuration will fall back to the following:
        - { window_title }:      PROJECT_INFO_STRING, i.e: CV v0.2.0 (Debug)
        - { window_width }:      800
        - { window_height }:     600
        - { desired_framerate }: 60
    */
    struct Engine_Config {
        std::string   root_dir          = "";
        std::string   window_title      = PROJECT_INFO_STRING;
        s16           window_width      = 800;
        s16           window_height     = 600;
        s16           desired_framerate = 60;
        Engine_Flags  flags             = Engine_Flags_None;

    #if PROJECT_ENGINE_BACKEND_DIRECTX
        HINSTANCE       instance;
        int             cmd_show;
    #endif
    };

    /*
        Validates engine configuration and initializes the specific engine backend.
        If correctly initialized the engine will start.
    */
    void
    initialize_and_start(Engine_Config config);


    struct s32x2 {
        s32 x;
        s32 y;

        s32x2(s32 x = 0, s32 y = 0)
        : x(x), y(y) {}
    };

    struct u8x4 {
        u8 x;
        u8 y;
        u8 z;
        u8 w;

        u8x4(u8 x = 0, u8 y = 0, u8 z = 0, u8 w = 0)
        : x(x), y(y), z(z), w(w) {}
    };

    struct f32x2 {
        f32 x;
        f32 y;

        f32x2(f32 x = 0.0f, f32 y = 0.0f)
        : x(x), y(y) {}
    };

    struct f32x4 {
        f32 x;
        f32 y;
        f32 z;
        f32 w;

        f32x4(f32 x = 0.0f, f32 y = 0.0f, f32 z = 0.0f, f32 w = 0.0f)
        : x(x), y(y), z(z), w(w) {}
    };

    // Components:
    typedef f32x4 Rect;
    typedef u8x4  Color;
    typedef f32x2 Velocity;

    /*
        Texture id is used to identify the GPU texture which will be used and rect a part of that texture to
        use.
    */
    struct Texture {
        int   id;
        f32x4 rect;

        Texture(int id = 0, f32x4 rect = {})
        : id(id), rect(rect) {}
    };

    // @todo: make the sound loading/playing happen in a separate thread since we do not need gfx context.
    struct Sound {
        int id;
        f32 volume;
        f32 pitch;

        Sound(int id = 0, f32 volume = 0.0f, f32 pitch = 0.0f)
        : id(id), volume(volume), pitch(pitch) {}
    };

    struct Font {
        int id;

        Font(int id = 0)
        : id(id) {}
    };

    struct Text {
        std::string data; //@todo: replace with a reusable buffer
        Font        font;
        Color       color;

        Text(std::string data = "", Font font = {}, Color color = {255,255,255,255})
        : data(data), font(font), color(color) {}
    };


    // Keys:
    enum Keyboard_Key {
        NONE = 0,

        KEY_A  = 65,
        KEY_B  = 66,
        KEY_C  = 67,
        KEY_D  = 68,
        KEY_E  = 69,
        KEY_F  = 70,
        KEY_G  = 71,
        KEY_H  = 72,
        KEY_I  = 73,
        KEY_J  = 74,
        KEY_K  = 75,
        KEY_L  = 76,
        KEY_M  = 77,
        KEY_N  = 78,
        KEY_O  = 79,
        KEY_P  = 80,
        KEY_Q  = 81,
        KEY_R  = 82,
        KEY_S  = 83,
        KEY_T  = 84,
        KEY_U  = 85,
        KEY_V  = 86,
        KEY_W  = 87,
        KEY_X  = 88,
        KEY_Y  = 89,
        KEY_Z  = 90
    };

    bool
    is_key_pressed(Keyboard_Key key);

    void
    set_clear_color(const u8x4& rgba_color);

    void
    draw_rect(const Rect& rect, const Color& fill_color);

    Texture
    load_texture(const std::string& texture_file_name);

    void
    draw_texture(Texture& texture, const Rect& entity_rect);

    Sound
    load_sound(const std::string& sound_file_name, f32 volume, f32 pitch);

    void
    play_sound(Sound& sound);

    Font
    load_font(const std::string& font_file_name, int font_size);

    void
    draw_text(const Text& text, f32x2 position);

    /*
    ## Common

        Set of common engine functions, implemented in { 2d_engine.cpp }.
    */

    std::string
    image_path(const std::string& file_name);

    std::string
    sound_path(const std::string& file_name);

    std::string
    font_path(const std::string& file_name);

    std::string
    model_path(const std::string& file_name);

    std::string
    model_path_glb(const std::string& file_name);


} // jbx
