
#pragma once

#include <vector>

namespace cv {

    /*
        Component_Mask is used to determine which components an entity has, or which components a system
        requires.
    */
    class Component_Mask {
    private:
        u16 value = 0;

    public:
        /*
            Check if { Component } of a given type { T } is in the mask.
        */
        template <typename T>
        bool has() const;

        /*
            Adds the { Component } of a given type { T } to the mask.
        */
        template <typename T>
        void add();

        /*
            Removes the { Component } of a given type { T } from the mask.
        */
        template <typename T>
        void remove();

        /*
            Get the current mask value:
        */
        inline const u16
        get() const {
            return value;
        }

        /*
            Resets the component signature, removing all the components.
        */
        inline void
        reset() {
            value = 0;
        }

        /*
            Check if given { Component_Mask } is contained within this mask.
        */
        inline bool
        contains(const Component_Mask& other) const {
            return (value & other.value) == other.value;
        }
    };

    constexpr int MAX_COMPONENTS = sizeof(Component_Mask) * 8;


    /*
        Entity is just an id.
    */
    struct Entity {
        int id;

        Entity(int id): id(id) {}
        Entity(const Entity& entity) = default;
        ~Entity()                    = default;

        bool
        operator ==(const Entity& other) const {
            return id == other.id;
        }

        bool
        operator <(const Entity& other) const {
            return id < other.id;
        }

        bool
        operator >(const Entity& other) const {
            return id > other.id;
        }
    };


    /*
        Just an interface to make it possible to automatically generate unique id's for every component type.

        For this we use a dirty trick in the subclass - Component, every generated version will increment the
        { next_id } value by 1. One caveat here is that we cannot guarantee that a component type will have
        the same ID every time, but that does not matter for our use case.
    */
    struct Base_Component {
    protected:
        static inline int next_id = 0;
    };

    template <typename T>
    class Component: public Base_Component {
    public:
        static int
        get_type_id() {
            static const int id = next_id++;
            ERROR_IF(id > MAX_COMPONENTS, "Maximum component count exceeded!");

            return id;
        }
    };


    /*
        A system simply keeps a collection of all the entities which meet the requirements and is open enough
        to enable it to be used in any part of the frame/lifecycle.
    */
    class Base_System {
    protected:
        Component_Mask       component_mask;
        std::vector<Entity>  entities;

    public:
        Base_System()  = default;
        ~Base_System() = default;

        void
        add_entity(Entity entity);

        void
        remove_entity(Entity entity);

        const Component_Mask&
        get_component_mask() const;
    };


    /*
        Just an interface which enables us to use the Pool with different types of components later.
    */
    class Base_Entity_Pool {
    public:
        virtual ~Base_Entity_Pool() {}

        virtual void
        remove_entity_from_pool(int entity_id) = 0;
    };

    /*
        Pool is just an abstraction over vector that we use in the Registry to store the entities which use
        these things.

        @todo: Simplify, reduce access time.
    */
    template <typename T>
    class Entity_Pool: public Base_Entity_Pool {
    private:
        std::vector<T> data;
        int            size;

        // @todo: simplify...
        std::unordered_map<int, int> entity_id_to_index;
        std::unordered_map<int, int> index_to_entity_id;

    public:
        Entity_Pool(int capacity = 128): size(0) {
            data.resize(capacity);
        }

        virtual ~Entity_Pool() = default;

        void
        add(T object) {
            data.push_back(object);
        }

        void
        set(int entity_id, T object) {
            ERROR_IF(entity_id < 0);

            // If this exists, just replace the object:
            if (entity_id_to_index.find(entity_id) != entity_id_to_index.end()) {
                int index = entity_id_to_index[entity_id];
                data[index] = object;
            } else {
                // Otherwise create a new entry:
                int index = size;
                entity_id_to_index.emplace(entity_id, index);
                index_to_entity_id.emplace(index, entity_id);

                // If there's not enough capacity, double it:
                if (index >= data.capacity()) {
                    data.resize(size * 2);
                }

                data[index] = object;
                size += 1;
            }
        }

        void
        remove(int entity_id) {
            ERROR_IF(size == 0, "The Pool is already empty!");

            // Swap the removed and the last:
            int index_of_removed    = entity_id_to_index[entity_id];
            int index_of_last       = size - 1;
            data[index_of_removed]  = data[index_of_last];

            // Update the index-entity mappings:
            int entity_id_of_last_element                   = index_to_entity_id[index_of_last];
            entity_id_to_index[entity_id_of_last_element]   = index_of_removed;
            index_to_entity_id[index_of_removed]            = entity_id_of_last_element;

            // Shrink:
            entity_id_to_index.erase(entity_id);
            index_to_entity_id.erase(index_of_last);
            size--;
        }

        void
        remove_entity_from_pool(int entity_id) override {
            if (entity_id_to_index.find(entity_id) != entity_id_to_index.end()) {
                remove(entity_id);
            }
        }

        T&
        get(int entity_id) {
            return static_cast<T&>(data[entity_id_to_index[entity_id]]);
        }

        T&
        operator [](int index) {
            ERROR_IF(index < 0);
            ERROR_IF(index >= data.size());

            return data[index];
        }
    };


    /*
        The ECS Registry keeps it all together, it's just a manager class that handles everything ECS related.
    */
    class Registry {
    private:
        int num_entities = 0;

        /*
            Each pool contains all the data for each component type, where { vector } index is the component
            type and { Pool } index is the entity id.
        */
        std::vector<Shared<Base_Entity_Pool>> component_pools;

        /*
            Signatures per entity, where index is the entity id.
        */
        std::vector<Component_Mask> component_masks;

        // Systems.
        std::unordered_map<std::type_index, Shared<Base_System>> systems;

        /*
            Entities to add/remove when able to do so, time is decoupled to allow us to make things more
            deterministic/simpler.
        */
        std::set<Entity> new_entities;
        std::set<Entity> dead_entities;

        // Entity groups:
        std::unordered_map<std::string, std::set<Entity>> entities_per_group;
        std::unordered_map<int, std::string>              groups_per_entity;

        // Free ids, which may be repurposed:
        std::deque<int> available_ids;

    public:
        Registry()  = default;
        ~Registry() = default;

        void
        update();

        Entity
        create_entity();

        /*
        ## Group management
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
        ## Component management
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
        void remove_system();

        template <typename T_System>
        T_System& get_system() const;

        void
        kill_entity(Entity entity);
    };

}

// Template implementations:
#include <2d_engine/ecs.inl>
