
// Implements:
#include <2d_engine/2d_engine.hpp>

// Dependencies:
#include <2d_engine/backend_hook.hpp>

// Dependencies (3rd party):
#include <filesystem>


namespace cv {

    /*
        Validate the configuration, if invalid fall back to reasonable defaults.
    */
    void
    initialize_and_start(Engine_Config config) {
        auto& validated_config = get_context<Engine_Config>();
        *validated_config = config;

        if (config.window_title.empty()) {
            log_warn("Empty string provided for window_title\n* Falling back to: \"{}\"", PROJECT_INFO_STRING);
            validated_config->window_title = PROJECT_INFO_STRING;
        }

        if (config.window_width <= 0 || config.window_height <= 0) {
            log_warn(
                "Invalid config value passed: window_size (x= {}, y= {})\n* Falling back to: (x= 800, y= 600)",
                config.window_width, config.window_height
            );

            validated_config->window_width = 800;
            validated_config->window_height = 600;
        }

        if (config.desired_framerate <= 0) {
            log_warn(
                "Invalid config value passed: desired_framerate= {}\n* Falling back to: 60",
                config.desired_framerate
            );

            validated_config->desired_framerate = 60;
        }

        if (config.root_dir.size() == 0) {
            log_warn("Empty config value passed for: root_dir\n* Falling back to: ./src/");
            config.root_dir = std::filesystem::current_path().string() + '/';
        }

        initialize_and_start_backend();
    }

} // cv
