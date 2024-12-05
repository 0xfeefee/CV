
// Implements:
#include <2d_engine/2d_engine.hpp>

// Dependencies:
#include <2d_engine/ecs.hpp>
#include <2d_engine/features/systems.hpp>
#include <2d_engine/backend_hook.hpp>
#include <2d_engine/2d_engine_api.hpp>

// Dependencies (3rd party):
#include <filesystem>
#include <string>


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
            config.root_dir = std::filesystem::current_path().string() + '/';
            log_warn("Empty config value passed for: root_dir\n* Falling back to: \"{}\"", config.root_dir);
        }

        /*
            Add every system to registry.
        */
        Unique<Registry>& registry = get_context<Registry>();
        registry->add_system<Rect_Renderer_System>();
        registry->add_system<Basic_Velocity_System>();
        registry->add_system<Texture_Renderer_System>();
        registry->add_system<Text_Renderer_System>();

        initialize_and_start_backend();
    }

    /*
        Resources intentionally only support 1 extension, in order to simplify and reduce scope further...
    */
    std::string
    image_path(const std::string& file_name) {
        return get_context<Engine_Config>()->root_dir + "assets/images/" + file_name + ".png";
    }

    std::string
    sound_path(const std::string& file_name) {
        return get_context<Engine_Config>()->root_dir + "assets/sounds/" + file_name + ".wav";
    }

    std::string
    font_path(const std::string& file_name) {
        return get_context<Engine_Config>()->root_dir + "assets/fonts/" + file_name + ".ttf";
    }

    std::string
    model_path(const std::string& file_name) {
        return get_context<Engine_Config>()->root_dir + "assets/models/" + file_name + ".obj";
    }

} // cv
