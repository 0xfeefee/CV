
#pragma once

// Implements:
#include <2d_engine/ecs.hpp>
#include <2d_engine/2d_engine_api.hpp>


namespace cv {

    struct Velocity {
        f32 horizontal;
        f32 vertical;

        Velocity(f32 horizontal = 0.0f, f32 vertical = 0.0f)
        : horizontal(horizontal), vertical(vertical) {
        }
    };

    class Example_System : public Base_System {
    public:
        Example_System() {
            component_mask.add<Velocity>();
        }

        void
        foobar() {
            Unique<Registry>& registry = get_context<Registry>();
            Entity e = registry->create_entity();
            registry->add_component<Velocity>(e, 42.0f, 42.0f);
        }

        void
        update() {
            Unique<Registry>& registry = get_context<Registry>();
            for (auto& entity: entities) {
                Velocity& c = registry->get_component<Velocity>(entity);
            }
        }
    };


    class Rect_Renderer_System : public Base_System {
    public:
        Rect_Renderer_System() {
            component_mask.add<Rect>();
            component_mask.add<Color>();

            log_warn("System comp mask: {}", component_mask);
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
