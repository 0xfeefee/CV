
#include <2d_engine/2d_engine.hpp>
#include <2d_engine/ecs.hpp>
#include <2d_engine/features/systems.hpp>

using namespace cv;

int
main(int argc, char* argv[]) {
    Component<u8>::get_type_id();
    // Component<u16>::get_type_id();
    // Component<u32>::get_type_id();
    // Component<u64>::get_type_id();

    // Component<s8>::get_type_id();
    // Component<s16>::get_type_id();
    // Component<s32>::get_type_id();
    // Component<s64>::get_type_id();

    // Component<f32>::get_type_id();
    // Component<f64>::get_type_id();

    Component_Mask mask;
    mask.add<s8>();
    mask.add<f32>();
    mask.add<std::string>();

    log_warn("mask: {}", mask);

    mask.reset();

    ERROR_IF(mask.has<s8>());
    ERROR_IF(mask.has<std::string>());

    log_warn("mask: {}", mask);

    log_warn(
        "{} {} {} {}, MAX_COMPONENT_TYPES: {}",
        Component<int>::get_type_id(),
        Component<float>::get_type_id(),
        Component<double>::get_type_id(),
        Component<std::string>::get_type_id(),
        MAX_COMPONENT_TYPES
    );

    Engine_Config config;
    config.desired_framerate = 144;
    initialize_and_start(config);

    return 0;
}
