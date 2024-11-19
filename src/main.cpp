
#include <2d_engine/2d_engine.hpp>
#include <2d_engine/ecs.hpp>
using namespace cv;

int
main(int argc, char* argv[]) {

    Component<u8>::get_id();
    Component<u16>::get_id();
    Component<u32>::get_id();
    Component<u64>::get_id();

    Component<s8>::get_id();
    Component<s16>::get_id();
    Component<s32>::get_id();
    Component<s64>::get_id();

    Component<f32>::get_id();
    Component<f64>::get_id();

    log_warn(
        "{} {} {} {}, MAX_COMPONENT_TYPES: {}",
        Component<int>::get_id(),
        Component<float>::get_id(),
        Component<double>::get_id(),
        Component<std::string>::get_id(),
        MAX_COMPONENTS
    );

    Engine_Config config;
    config.desired_framerate = 144;
    initialize_and_start(config);

    return 0;
}
