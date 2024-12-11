
#pragma once
#include <ecs/ecs.hpp>
#include <2d_engine/2d_engine_api.hpp>

namespace jbx {

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
        update(f64 dt) override {
            Unique<Registry>& registry = get_context<Registry>();
            for (auto& entity: entities) {
                Rect& transform   = registry->get_component<Rect>(entity);
                Color& tint_color = registry->get_component<Color>(entity);

                draw_rect(transform, tint_color);
            }
        }
    };

} // jbx