
// Dependencies:
#include <2d_engine/2d_engine.hpp>
using namespace cv;

/*
    For DirectX we use WinMain entry point, other backends use a good old main.
*/
#ifdef PROJECT_ENGINE_BACKEND_DIRECTX
    #include <windows.h>

    int
    WinMain(HINSTANCE instance, HINSTANCE previous_instance, LPSTR cmd_string, int cmd_show) {
        Engine_Config config;
        config.desired_framerate = 144;
        config.window_width      = 1920;
        config.window_height     = 1080;
        config.flags             = Engine_Flags_No_Decoration;
        config.instance          = instance;
        config.cmd_show          = cmd_show;

        initialize_and_start(config);
        return 0;
    }
#else
    int
    main(int argc, cstr_t argv[]) {
        // User may supply a root directory, this is useful for testing:
        cstr_t user_root_dir = "";
        if (argc > 1) {
            user_root_dir = argv[1];
        }

        // Configure & run:
        Engine_Config config;
        config.desired_framerate = 144;
        config.root_dir          = user_root_dir;
        config.flags             = Engine_Flags_No_Decoration;

        initialize_and_start(config);
        return 0;
    }
#endif
