
#pragma once
#include <ecs/ecs.hpp>
#include <2d_engine/2d_engine_api.hpp>

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

        ~Basic_Velocity_System() = default;

        void
        update(f64 delta_time) override {
            Unique<Registry>& registry = get_context<Registry>();
            for (auto& entity: entities) {
                Rect& transform    = registry->get_component<Rect>(entity);
                Velocity& velocity = registry->get_component<Velocity>(entity);

                transform.x += velocity.x * delta_time;
                transform.y += velocity.y * delta_time;
            }
        }
    };

} // jbx
