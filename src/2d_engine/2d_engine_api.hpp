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

    void
    set_clear_color(const u8x4& rgba_color);

    void
    draw_rect(const Rect& rect, const Color& fill_color);

} // cv
