
// Implements:
#include <features/Basic_Velocity_System.hpp>

// Dependencies:
#include <engine/core/engine.hpp>

namespace jbx {

    Basic_Velocity_System::Basic_Velocity_System() {
        component_mask.add<Rect>();
        component_mask.add<Velocity>();
    }

    void
    Basic_Velocity_System::update(f64 delta_time) {
        Unique<Registry>& registry = get_context<Registry>();
        for (auto& entity: entities) {
            Rect& transform    = registry->get_component<Rect>(entity);
            Velocity& velocity = registry->get_component<Velocity>(entity);

            transform.x += velocity.x * delta_time;
            transform.y += velocity.y * delta_time;
        }
    }

} // jbx