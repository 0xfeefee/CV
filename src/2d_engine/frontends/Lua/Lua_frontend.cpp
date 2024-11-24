
// Implements:
#include <2d_engine/frontend_hook.hpp>

#include <2d_engine/2d_engine.hpp>

#define SOL_NO_EXCEPTIONS 1
#include <sol/sol.hpp> // Includes Lua.

namespace cv {

    struct Script_Context {
        sol::function begin;
        sol::function step;
        sol::function end;
    };


    void
    frontend_start() {
        sol::state& lua = *get_context<sol::state>();

        /*
            Engine will load Lua sources from the { src } directory relative to the current working directory.
        */
        std::string project_root_dir = get_context<Engine_Config>()->root_dir;
        std::string project_pattern  = fmt::format(";{}src/?.lua", project_root_dir);
        std::string project_main     = fmt::format("{}src/main.lua", project_root_dir);

        log_warn(
            "project_root_dir: {}\nproject_pattern:{}\nproject_main: {}\n",
            project_root_dir,
            project_pattern,
            project_main
        );

        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::os);
        lua["package"]["path"] = lua["package"]["path"].get<std::string>() + project_pattern;

        // Validate the main script:
        sol::load_result script = lua.load_file(project_main);
        if (!script.valid()) {
            sol::error err = script;
            log_error("Lua error: {}", err.what());
        }

        // Run the script:
        lua.script_file(project_main);

        /*
            Setup the bindings for lifecycle functions and provide defaults when user does not define them!
            This is important because sometimes you may not need { game_begin, game_end }. Without { game_step }
            you can still create something static, so defaults for that exist too!

            Defaults will not print anything in the final version, because it's rather annoying to spam that
            without a choice.
        */
        Unique<Script_Context>& context = get_context<Script_Context>();

        if (!lua["game_begin"].valid()) {
            cstr_t default_begin = R"(
                function game_begin()
                    print("Default begin!")
                end
            )";
            lua.script(default_begin);
        }

        if (!lua["game_step"].valid()) {
            cstr_t default_step = R"(
                function game_step(dt)
                    print("Default step!")
                end
            )";
            lua.script(default_step);
        }

        if (!lua["game_end"].valid()) {
            cstr_t default_end = R"(
                function game_end()
                    print("Default end!")
                end
            )";
            lua.script(default_end);
        }

        context->begin  = lua["game_begin"];
        context->step   = lua["game_step"];
        context->end    = lua["game_end"];

        // Run user { begin } function:
        context->begin();
    }

    void
    frontend_step(f64 delta_time) {
        Unique<Script_Context>& context = get_context<Script_Context>();

        // Run user { step } function:
        context->step(delta_time);
    }

    void
    frontend_stop() {
        Unique<Script_Context>& context = get_context<Script_Context>();

        // Run user { end } function:
        context->end();
    }

}
