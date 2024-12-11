
// Implements:
#include <features/Rect_Renderer_System.hpp>

// Dependencies:
#include <ecs/ecs.hpp>
#include <2d_engine/2d_engine_api.hpp>

namespace jbx {

    Rect_Renderer_System::Rect_Renderer_System() {
        component_mask.add<Rect>();
        component_mask.add<Color>();
    }

    void
    Rect_Renderer_System::update(f64 delta_time) {
        Unique<Registry>& registry = get_context<Registry>();
        for (auto& entity: entities) {
            Rect& transform   = registry->get_component<Rect>(entity);
            Color& tint_color = registry->get_component<Color>(entity);

            draw_rect(transform, tint_color);
        }
    }

} // jbx