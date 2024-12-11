
// Implements:
#include <ecs/Base_System.hpp>

namespace jbx {

    /*
    ## Base_System: implementation
    */

    void
    Base_System::add_entity(Entity entity) {
        entities.push_back(entity);
    }

    void
    Base_System::remove_entity(Entity entity) {
        for (auto it = entities.begin(); it != entities.end();) {
            Entity& current = *it;
            if (current == entity) {
                it = entities.erase(it);
                break;
            } else {
                it += 1;
            }
        }
    }

    const Component_Mask&
    Base_System::get_component_mask() const {
        return component_mask;
    }

}
