
// Implements:
#include <engine/core/frontend_hook.hpp>

// Dependencies:
#include <engine/core/engine.hpp>
#include <ecs/ecs.hpp>

// Dependencies (3rd_party):
#define SOL_NO_EXCEPTIONS 1
#include <sol/sol.hpp>
/*
@todo: Factor this out so it's less messy, separate the bindings from the core.
@todo: Make the user experience better, now it feels a bit janky....
*/
namespace jbx {

    struct Script_Context {
        sol::function begin;
        sol::function step;
        sol::function end;
    };

    static Rect&
    get_rect(const Entity& entity) {
        return get_context<Registry>()->get_component<Rect>(entity);
    }

    static Color&
    get_color(const Entity& entity) {
        return get_context<Registry>()->get_component<Color>(entity);
    }

    static Velocity&
    get_velocity(const Entity& entity) {
        return get_context<Registry>()->get_component<Velocity>(entity);
    }

    static Texture&
    get_texture(const Entity& entity) {
        return get_context<Registry>()->get_component<Texture>(entity);
    }

    /*
        Create an entity from the entity definition.
        - Component is added to the entity if any of the component fields are specified in the definition.
        - Most default to zero but some are required, if for instance you specify any of the fields of { Rect }
          component, then you must also make sure to include non-zero { width, height } fields. For now this
          raises an error in debug builds only, but we will have a similar behavior for release builds too.
    */
    static sol::table
    create_entity(sol::table& def) {
        Unique<Registry>& registry = get_context<Registry>();
        Entity entity              = registry->create_entity();

        sol::table lua_entity_info = get_context<sol::state>()->create_table();
        lua_entity_info["id"]      = entity;

        // Rect:
        if (def["x"].valid() || def["y"].valid() || def["width"].valid() || def["height"].valid()) {
            lua_entity_info["rect"] = true;

            ERROR_IF(
                def["width"].get_or(0.0f) <= 0 || def["height"].get_or(0.0f) <= 0,
                "Must specify both { width } and { height } of an entity!"
            );

            registry->add_component<Rect>(
                entity,
                def["x"].get_or(0.0f),
                def["y"].get_or(0.0f),
                def["width"].get_or(0.0f),
                def["height"].get_or(0.0f)
            );
        }

        // Velocity:
        if (def["hspeed"].valid() || def["vspeed"].valid()) {
            lua_entity_info["velocity"] = true;
            registry->add_component<Velocity>(
                entity,
                def["hspeed"].get_or(0.0f),
                def["vspeed"].get_or(0.0f)
            );
        }

        // Color:
        if (def["r"].valid() || def["g"].valid() || def["b"].valid() || def["a"].valid()) {
            lua_entity_info["color"] = true;
            registry->add_component<Color>(
                entity,
                def["r"].get_or(0),
                def["g"].get_or(0),
                def["b"].get_or(0),
                def["a"].get_or(255)
            );
        }

        // Texture:
        if (def["texture_id"].valid()) {
            lua_entity_info["texture"] = true; // We might not even support this ...
            registry->add_component<Texture>(
                entity,
                def["texture_id"].get_or(0),
                f32x4(
                    def["texture_x"].get_or(0.0f),
                    def["texture_y"].get_or(0.0f),
                    def["texture_width"].get_or(0.0f),
                    def["texture_height"].get_or(0.0f)
                )
            );
        }

        // Text
        if (def["text"].valid()) {
            lua_entity_info["text"] = true;
            registry->add_component<Text>(
                entity,
                def["text"].get_or(std::string("Lorem Ipsum")),
                def["font"].get_or(Font()),
                Color(
                    def["font_r"].get_or(0),
                    def["font_g"].get_or(0),
                    def["font_b"].get_or(0),
                    def["font_a"].get_or(255)
                )
            );
        }

        return lua_entity_info;
    }

    static inline void
    bind_engine_api(sol::state& lua) {
        /*
        ## Types
        */

        lua.new_usertype<Color>(
            "Color",
            sol::constructors<Color(), Color(u8, u8, u8), Color(u8, u8, u8, u8)>(),
            "r", &Color::x,
            "g", &Color::y,
            "b", &Color::z,
            "a", &Color::w
        );

        lua.new_usertype<Velocity>(
            "Velocity",
            sol::constructors<Velocity(), Velocity(f32, f32)>(),
            "x", &Velocity::x,
            "y", &Velocity::y
        );

        lua.new_usertype<Entity>(
            "Entity",
            "id", &Entity::id
        );

        lua.new_usertype<Rect>(
            "Rect",
            "x",      &Rect::x,
            "y",      &Rect::y,
            "width",  &Rect::z,
            "height", &Rect::w
        );

        lua.new_usertype<Texture>(
            "Texture",
            sol::constructors<Texture(int, f32x4)>(),
            "id",   &Texture::id,
            "rect", &Texture::rect
        );

        lua.new_usertype<Sound>(
            "Sound",
            sol::constructors<Sound(int, f32, f32)>(),
            "id",     &Sound::id,
            "volume", &Sound::volume,
            "pitch",  &Sound::pitch
        );

        lua.new_usertype<Font>(
            "Font",
            sol::constructors<Font(int)>(),
            "id", &Font::id
        );

        lua.new_usertype<Text>(
            "Text",
            sol::constructors<Text(std::string, Font, Color)>(),
            "data", &Text::data,
            "font", &Text::font,
            "color", &Text::color
        );

        lua.new_enum<Keyboard_Key>("Key", {
            { "A", Keyboard_Key::KEY_A },
            { "B", Keyboard_Key::KEY_B },
            { "C", Keyboard_Key::KEY_C },
            { "D", Keyboard_Key::KEY_D },
            { "E", Keyboard_Key::KEY_E },
            { "F", Keyboard_Key::KEY_F },
            { "G", Keyboard_Key::KEY_G },
            { "H", Keyboard_Key::KEY_H },
            { "I", Keyboard_Key::KEY_I },
            { "J", Keyboard_Key::KEY_J },
            { "K", Keyboard_Key::KEY_K },
            { "L", Keyboard_Key::KEY_L },
            { "M", Keyboard_Key::KEY_M },
            { "N", Keyboard_Key::KEY_N },
            { "O", Keyboard_Key::KEY_O },
            { "P", Keyboard_Key::KEY_P },
            { "Q", Keyboard_Key::KEY_Q },
            { "R", Keyboard_Key::KEY_R },
            { "S", Keyboard_Key::KEY_S },
            { "T", Keyboard_Key::KEY_T },
            { "U", Keyboard_Key::KEY_U },
            { "V", Keyboard_Key::KEY_V },
            { "W", Keyboard_Key::KEY_W },
            { "X", Keyboard_Key::KEY_X },
            { "Y", Keyboard_Key::KEY_Y },
            { "Z", Keyboard_Key::KEY_Z }
        });

        /*
        ## Functions
        */
        sol::table api_bindings = lua.create_table(); // @todo make this namespace the types too...

        // Lua specific:
        api_bindings.set_function("create_entity", create_entity);
        api_bindings.set_function("get_rect", get_rect);
        api_bindings.set_function("get_color", get_color);
        api_bindings.set_function("get_velocity", get_velocity);
        api_bindings.set_function("get_texture", get_texture);

        // Directly from engine API:
        api_bindings.set_function("clear_color", set_clear_color);
        api_bindings.set_function("is_key_pressed", is_key_pressed);
        api_bindings.set_function("load_texture", load_texture);
        api_bindings.set_function("load_sound", load_sound);
        api_bindings.set_function("play_sound", play_sound);
        api_bindings.set_function("load_font", load_font);

        lua["cv"] = api_bindings;
    }

    static inline void
    preload_common_api(sol::state& lua) {
        lua.script(R"(
            function create_entity(def)
                local entity = cv.create_entity(def)

                if entity.rect then
                    entity.rect = cv.get_rect(entity.id)
                end

                if entity.color then
                    entity.color = cv.get_color(entity.id)
                end

                if entity.velocity then
                    entity.velocity = cv.get_velocity(entity.id)
                end

                if entity.texture then
                    entity.texture = cv.get_texture(entity.id)
                end

                return entity
            end
        )");
    }

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

        // Bind the API:
        bind_engine_api(lua);
        preload_common_api(lua);

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

} // jbx
