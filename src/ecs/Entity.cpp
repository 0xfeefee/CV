// Implements:
#include <ecs/Entity.hpp>

namespace jbx {

    bool
    Entity::operator==(const Entity& other) const {
        return id == other.id;
    }

    bool
    Entity::operator<(const Entity& other) const {
        return id < other.id;
    }

    bool
    Entity::operator>(const Entity& other) const {
        return id > other.id;
    }

} // jbx
