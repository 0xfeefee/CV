
#include <2d_engine/2d_engine.hpp>
using namespace cv;

int
main(int argc, char* argv[]) {
    Engine_Config config;
    config.desired_framerate = 144;
    initialize_and_start(config);
    return 0;
}
