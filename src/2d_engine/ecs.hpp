#pragma once
/*
    Simple and somewhat naive ECS implementation, which should be good enough to handle the small games for
    which this engine is built.

    ECS supports up to { sizeof(Component_Mask) } unique component types and up to { N } entities...
    @todo: set a hard limit on entity count.
*/
namespace cv {

    #define get_type_index(type)        std::type_index(typeid(type))

    /*
        Component_Mask is used to determine which components an entity has, or which components a system
        requires.
    */
    class Component_Mask {
    private:
        u8 value = 0;

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
        const u16
        get_value() const;

        /*
            Resets the component signature, removing all the components.
        */
        void
        reset();

        /*
            Check if given { Component_Mask } is contained within this mask.
        */
        bool
        contains(const Component_Mask& other) const;
    };


    /*
        We should not need more than 32 different types of { Component } in order to cover all the feature this
        engine needs.
    */
    constexpr int MAX_COMPONENT_TYPES = sizeof(Component_Mask) * 8;


    /*
        Entity is just an id.
    */
    struct Entity {
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


    /*
        Just an interface to make it possible to automatically generate unique id's for every component type.

        For this we use a dirty trick in the subclass - { Component } every generated version will increment the
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
        /*
            This determines which bit in the { Component_Mask } is used represent the presence of this specific
            component type { T }.
        */
        static int
        get_type_id();
    };


    /*
        A system simply keeps a collection of all the entities which meet the component requirements.

        { Base_System } is purposefully open-ended to enable systems to be used in different ways or different
        parts of the life-cycle.
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
    class Base_Pool {
    public:
        virtual ~Base_Pool() {}

        virtual void
        remove_entity_from_pool(int entity_id) = 0;
    };

    /*
        { Pool } is just an abstraction over vector that we use in the { Registry } to store the data.
        @todo: Simplify, reduce access time.
    */
    template <typename T>
    class Pool: public Base_Pool {
    private:
        std::vector<T> data;
        int            size;

        std::unordered_map<int, int> entity_id_to_index;
        std::unordered_map<int, int> index_to_entity_id;

    public:
        Pool(int capacity = 128);

        virtual
        ~Pool() = default;

        void
        add(T object);

        void
        set(int entity_id, T object);

        void
        remove(int entity_id);

        void
        remove_entity_from_pool(int entity_id) override;

        T&
        get(int entity_id);

        T&
        operator [](int index);
    };


    /*
        The ECS Registry keeps it all together, it's just a manager class that handles everything ECS related.
    */
    class Registry {
    private:
        int num_entities = 0;

        /*
            Each pool contains all the data for each component type, where:
            - { vector } index is the component type
            - { Base_Pool } index is the entity id
        */
        std::vector<Shared<Base_Pool>> component_pools;

        /*
            Signatures per entity, where index is the entity id.
        */
        std::vector<Component_Mask> component_masks;

        // All systems:
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

}

// Template implementations:
#include <2d_engine/ecs.inl>
