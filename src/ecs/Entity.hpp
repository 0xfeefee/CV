
#pragma once

namespace jbx {

    /*
        Entity is just an id.
    */
    struct Entity final {
        int id;

        Entity(int id): id(id) {}
        Entity(const Entity& entity) = default;
        ~Entity()                    = default;

        bool
        operator ==(const Entity& other) const;

        bool
        operator <(const Entity& other) const;

        bool
        operator >(const Entity& other) const;
    };

} // jbx
