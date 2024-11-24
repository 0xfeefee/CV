
#include <2d_engine/2d_engine.hpp>

int
main(int argc, char* argv[]) {
    using namespace cv;

    // User may supply a root directory, this is useful for testing:
    cstr_t user_root_dir = "";
    if (argc > 1) {
        user_root_dir = argv[1];
    }

    // Configure & run:
    Engine_Config config;
    config.desired_framerate = 144;
    config.root_dir          = user_root_dir;

    initialize_and_start(config);
    return 0;
}
