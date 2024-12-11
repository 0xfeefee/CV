
#pragma once

#include <ecs/Entity.hpp>
#include <ecs/Component.hpp>
#include <ecs/Base_System.hpp>
#include <ecs/Pool.hpp>

namespace jbx {

    #define get_type_index(type)        std::type_index(typeid(type))

    /*
        The ECS Registry keeps it all together, it's just a manager class that handles
        everything ECS related.

        { component_pools }: contain all the data for each component type, where:
        - { vector } is the index of component type.
        - { Base_Pool } index is the entity id.

        Perhaps we can move the { Component_Mask } into the entity struct itself, since it's
        most commonly used.

        @todo: profile to see if it actually matters ...
    */
    class Registry final {
    private:
        int num_entities = 0;

        std::vector<Shared<Base_Pool>> component_pools;
        std::vector<Component_Mask>    component_masks;

        std::unordered_map<std::type_index, Shared<Base_System>> systems;

        std::set<Entity> new_entities;
        std::set<Entity> dead_entities;

        std::unordered_map<std::string, std::set<Entity>> entities_per_group;
        std::unordered_map<int, std::string>              groups_per_entity;

        std::deque<int> available_ids;

    public:
        Registry()  = default;
        ~Registry() = default;

        void
        update();

        Entity
        create_entity();

        /*
        ## Group management:
        */

        void
        group_entity(Entity entity, const std::string& group);

        bool
        entity_belongs_to_group(Entity entity, const std::string& group) const;

        std::vector<Entity>
        get_entities_by_group(const std::string& group) const;

        void
        ungroup_entity(Entity entity);

        /*
        ## Component management:
        */

        template <typename T_Component, typename ...T_Component_Args>
        void add_component(Entity entity, T_Component_Args&& ...args);

        template <typename T_Component>
        void remove_component(Entity entity);

        template <typename T_Component>
        T_Component& get_component(Entity entity);

        template <typename T_System, typename ...T_System_Args>
        void add_system(T_System_Args&& ...args);

        template <typename T_System>
        T_System& get_system() const;

        void
        kill_entity(Entity entity);

        const Component_Mask&
        get_component_mask(Entity entity) const;
    };


    /*
    ## Registry: template implementations
    */

    template <typename T_Component, typename ...T_Component_Args>
    void Registry::add_component(Entity entity, T_Component_Args&& ...args) {
        // Get the id's:
        int component_type_id = Component<T_Component>::get_type_id();
        int entity_id         = entity.id;

        ERROR_IF(
            component_type_id >= MAX_COMPONENT_TYPES,
            "We have exceeded the maximum engine supported unique component types!"
        );

        // If we are by chance out of the component pool boundaries:
        if (component_type_id >= component_pools.size())    {
            component_pools.resize(component_type_id + 1, nullptr);
        }

        // If the pool is not initialized, initialize it:
        if (component_pools[component_type_id] == nullptr) {
            Shared<Pool<T_Component>> new_component_pool = std::make_shared<Pool<T_Component>>();
            component_pools[component_type_id]           = new_component_pool;
        }

        // Get the pool of correct type, component_pools are Base_Pool*.
        Shared<Pool<T_Component>> component_pool = std::static_pointer_cast<Pool<T_Component>>(
            component_pools[component_type_id]
        );

        // Create a new component of this type & add it to the component pool:
        T_Component new_component(std::forward<T_Component_Args>(args)...);
        component_pool->set(entity_id, new_component);

        // Update the component mask for this entity:
        component_masks[entity_id].add<T_Component>();
    }

    /*
        Remove the component from an entity, we don't need to check if entity has the component
        at all because all we need to do is set the correct bit to 0:
    */
    template <typename T_Component>
    void Registry::remove_component(Entity entity) {
        // Get the id's:
        const int component_type_id = Component<T_Component>::get_type_id();
        const int entity_id         = entity.id;

        ERROR_IF(
            component_type_id >= MAX_COMPONENT_TYPES,
            "We have exceeded the maximum engine supported unique component types!"
        );

        ERROR_IF(
            component_masks.size() <= entity_id,
            "Out of bounds! Entity with the given id is not registered!"
        );

        // Remove the component from the component pool for that entity:
        Shared<Pool<T_Component>> component_pool = std::static_pointer_cast<Pool<T_Component>>(
            component_pools[component_type_id]
        );
        component_pool->remove(entity_id);

        // Set the bit
        component_masks[entity_id].remove<T_Component>();
    }

    template <typename T_Component>
    T_Component&
    Registry::get_component(Entity entity) {
        // Get the ids:
        const int component_type_id = Component<T_Component>::get_type_id();
        const int entity_id         = entity.id;

        // Make sure we have it first:
        ERROR_IF(
            component_masks[entity_id].has<T_Component>() == false,
            "Entity does not have this component!"
        );

        /*
            Get the correct pool of components and fetch the instance of the component from the
            pool:
        */
        Shared<Pool<T_Component>> pool = std::static_pointer_cast<Pool<T_Component>>(
            component_pools[component_type_id]
        );

        return pool->get(entity_id);
    }

    template <typename T_System, typename ...T_System_Args>
    void Registry::add_system(T_System_Args&& ...args) {
        Shared<T_System> new_system = std::make_shared<T_System>(std::forward<T_System_Args>(args)...);
        auto system_type_index      = get_type_index(T_System);

        // Add it to the map:
        systems.insert(std::make_pair(system_type_index, new_system));
    }

    template <typename T_System>
    T_System& Registry::get_system() const {
        auto system_iter = systems.find(get_type_index(T_System));

        // @note: this is a bit cursed, maybe we should expand it out into multple statements for clarity...
        T_System& system_ref = *(std::static_pointer_cast<T_System>(system_iter->second));
        return system_ref;
    }

} // jbx
