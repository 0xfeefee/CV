#pragma once

namespace cv {

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

        f32x2(f32 x = 0, f32 y = 0)
        : x(x), y(y) {}
    };

    struct f32x4 {
        f32 x;
        f32 y;
        f32 z;
        f32 w;

        f32x4(f32 x = 0, f32 y = 0, f32 z = 0, f32 w = 0)
        : x(x), y(y), z(z), w(w) {}
    };

    // Components:
    typedef f32x4 Rect;
    typedef u8x4  Color;
    typedef f32x2 Velocity;

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

    void
    set_clear_color(const u8x4& rgba_color);

    void
    draw_rect(const Rect& rect, const Color& fill_color);

    bool
    is_key_pressed(Keyboard_Key key);

} // cv
