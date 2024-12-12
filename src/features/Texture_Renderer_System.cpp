
// Implements:
#include <features/Texture_Renderer_System.hpp>

// Dependencies:
#include <engine/core/engine.hpp>

namespace jbx {

    Texture_Renderer_System::Texture_Renderer_System() {
        component_mask.add<Rect>();
        component_mask.add<Texture>();
    }

    void
    Texture_Renderer_System::update(f64 delta_time) {
        Unique<Registry>& registry = get_context<Registry>();
        for (auto& entity: entities) {
            Rect& rect       = registry->get_component<Rect>(entity);
            Texture& texture = registry->get_component<Texture>(entity);

            draw_texture(texture, rect);
        }
    }

} // jbx
