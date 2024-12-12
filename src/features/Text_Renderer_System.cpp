
// Implements:
#include <features/Text_Renderer_System.hpp>

// Dependencies:
#include <engine/core/engine.hpp>

namespace jbx {

    Text_Renderer_System::Text_Renderer_System() {
        component_mask.add<Rect>();
        component_mask.add<Text>();
    }

    void
    Text_Renderer_System::update(f64 delta_time) {
        Unique<Registry>& registry = get_context<Registry>();
        for (auto& entity: entities) {
            Rect& rect = registry->get_component<Rect>(entity);
            Text& text = registry->get_component<Text>(entity);

            draw_text(text, {rect.x, rect.y});
        }
    }

} // jbx
