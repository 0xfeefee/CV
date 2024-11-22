#pragma once

namespace cv {

    /*
        Flags used to configure the platform windows
        - { Window_Flags_None }:       Nothing.
        - { Window_Flags_Vsync }:      Enable VSYNC on the platform window.
        - { Window_Flags_Fullscreen }: Set the platform window video mode to fullscreen.
        - { Window_Flags_Borderless }: Remove all decorations from the platform window.
    */
    typedef u8 Engine_Flags;
    enum Engine_Flags_ : u8 {
        Engine_Flags_None       = 0,        // 0000 0000
        Engine_Flags_Vsync      = 1 << 0,   // 0000 0001
        Engine_Flags_Fullscreen = 1 << 1,   // 0000 0010
        Engine_Flags_Borderless = 1 << 2    // 0000 0100
    };

    /*
        Configure the engine, when unspecified or invalid the configuration will fall back to the following:
        - { window_title }:      PROJECT_INFO_STRING, i.e: CV v0.2.0 (Debug)
        - { window_width }:      800
        - { window_height }:     600
        - { desired_framerate }: 60
    */
    struct Engine_Config {
        std::string   root_dir          = "./src";
        std::string   window_title      = PROJECT_INFO_STRING;
        s16           window_width      = 800;
        s16           window_height     = 600;
        s16           desired_framerate = 60;
        Engine_Flags  flags             = Engine_Flags_None;
    };

    /*
        Validates engine configuration and initializes the specific engine backend.
        If correctly initialized the engine will start.
    */
    void
    initialize_and_start(Engine_Config config);

} // cv

