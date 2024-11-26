
#pragma once

// Implements:
#include <2d_engine/ecs.hpp>
#include <2d_engine/2d_engine_api.hpp>


namespace cv {

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
                Rect& transform = registry->get_component<Rect>(entity);
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

}
