
#pragma once

// Implements:
#include <ecs/ecs.hpp>

// Dependencies:
#include <2d_engine/2d_engine_api.hpp>
/*
    @todo: the painters algorithm needs z-sorting...
*/
namespace jbx {

    /*
        Modifies entity position based on the current velocity...
    */
    class Basic_Velocity_System : public Base_System {
    public:
        Basic_Velocity_System() {
            component_mask.add<Rect>();
            component_mask.add<Velocity>();
        }

        void
        update(f64 dt) {
            Unique<Registry>& registry = get_context<Registry>();
            for (auto& entity: entities) {
                Rect& transform    = registry->get_component<Rect>(entity);
                Velocity& velocity = registry->get_component<Velocity>(entity);

                transform.x += velocity.x * dt;
                transform.y += velocity.y * dt;
            }
        }
    };


    /*
        Draws an entity rectangle...
    */
    class Rect_Renderer_System : public Base_System {
    public:
        Rect_Renderer_System() {
            component_mask.add<Rect>();
            component_mask.add<Color>();
        }

        void
        update() {
            Unique<Registry>& registry = get_context<Registry>();
            for (auto& entity: entities) {
                Rect& transform   = registry->get_component<Rect>(entity);
                Color& tint_color = registry->get_component<Color>(entity);

                draw_rect(transform, tint_color);
            }
        }
    };

    /*
        Draws a texture, or a subtexture for the given entity.
    */
    class Texture_Renderer_System : public Base_System {
    public:
        Texture_Renderer_System() {
            component_mask.add<Rect>();
            component_mask.add<Texture>();
        }

        void
        update() {
            Unique<Registry>& registry = get_context<Registry>();
            for (auto& entity: entities) {
                Rect& rect       = registry->get_component<Rect>(entity);
                Texture& texture = registry->get_component<Texture>(entity);

                draw_texture(texture, rect);
            }
        }
    };

    class Text_Renderer_System : public Base_System {
    public:
        Text_Renderer_System() {
            component_mask.add<Text>();
            component_mask.add<Rect>();
        }

        void
        update() {
            Unique<Registry>& registry = get_context<Registry>();
            for (auto& entity: entities) {
                Rect& rect = registry->get_component<Rect>(entity);
                Text& text = registry->get_component<Text>(entity);

                draw_text(text, {rect.x, rect.y});
            }
        }
    };

} // jbx
