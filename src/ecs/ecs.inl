
namespace jbx {

	/*
	## Component_Mask: template implementations
	*/

	template <typename T>
	bool Component_Mask::has() const {
	    int component_type_id = Component<T>::get_type_id();
	    return (value & (1 << component_type_id)) != 0;
	}

	template <typename T>
	void
	Component_Mask::add() {
	    int component_type_id = Component<T>::get_type_id();
	    value |= (1 << component_type_id);
	}

	template <typename T>
	void
	Component_Mask::remove() {
	    int component_type_id = Component<T>::get_type_id();
	    value &= ~(1 << component_type_id);
	}


    /*
    ## Component: template implementations
    */

    template <typename T>
    int
    Component<T>::get_type_id() {
        static const int id = next_id++;
        ERROR_IF(id > MAX_COMPONENT_TYPES, "Maximum component count exceeded!");

        return id;
    }


    /*
    ## Base_Pool: template implementations
    */

    template <typename T>
    Pool<T>::Pool(int capacity): size(0) {
        data.resize(capacity);
    }

    template <typename T>
    void
    Pool<T>::add(T object) {
        data.push_back(object);
    }

    template <typename T>
    void
    Pool<T>::set(int entity_id, T object) {
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

    template <typename T>
    void
    Pool<T>::remove(int entity_id) {
        ERROR_IF(size == 0, "The Pool is already empty!");

        // Swap the removed and the last:
        int index_of_removed   = entity_id_to_index[entity_id];
        int index_of_last      = size - 1;
        data[index_of_removed] = data[index_of_last];

        // Update the index-entity mappings:
        int entity_id_of_last_element                 = index_to_entity_id[index_of_last];
        entity_id_to_index[entity_id_of_last_element] = index_of_removed;
        index_to_entity_id[index_of_removed]          = entity_id_of_last_element;

        // Shrink:
        entity_id_to_index.erase(entity_id);
        index_to_entity_id.erase(index_of_last);
        size--;
    }

    template <typename T>
    void
    Pool<T>::remove_entity_from_pool(int entity_id) {
        if (entity_id_to_index.find(entity_id) != entity_id_to_index.end()) {
            remove(entity_id);
        }
    }

    template <typename T>
    T&
    Pool<T>::get(int entity_id) {
        return static_cast<T&>(data[entity_id_to_index[entity_id]]);
    }

    template <typename T>
    T&
    Pool<T>::operator[](int index) {
        EXPECT(index > 0 && index < data.size);
        return data[index];
    }


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
        Remove the component from an entity, we don't need to check if entity has the component at all because
        all we need to do is set the correct bit to 0:
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

        // Get the correct pool of components and fetch the instance of the component from the pool:
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

    /*
        @note: Maybe we should just make it possible to get null in this case instead, because forcing a call to
        has_xyz is not fun.
    */
    template <typename T_System>
    T_System& Registry::get_system() const {
        auto system_iter = systems.find(get_type_index(T_System));

        // @note: this is a bit cursed, maybe we should expand it out into multple statements for clarity...
        T_System& system_ref = *(std::static_pointer_cast<T_System>(system_iter->second));
        return system_ref;
    }

} // jbx


#if PROJECT_ENABLE_LOGS
    /*
        If logs are enabled make it easy for { fmt } and by extension { spdlog } to print out the Component_Mask
        correctly.
    */
	template <>
	struct fmt::formatter<jbx::Component_Mask> {

	    inline constexpr auto
	    parse(fmt::format_parse_context& ctx) {
	        return ctx.begin();
	    }

	    template <typename FormatContext>
	    inline auto
	    format(const jbx::Component_Mask& mask, FormatContext& ctx) {
            if constexpr (sizeof(jbx::Component_Mask) == 1) {        // 8 bits
                return fmt::format_to(ctx.out(), "[{:08b}]", mask.get_value());
            } if constexpr (sizeof(jbx::Component_Mask) == 2) {      // 16 bits
                return fmt::format_to(ctx.out(), "[{:016b}]", mask.get_value());
            } else if constexpr (sizeof(jbx::Component_Mask) == 4) { // 32 bits
                return fmt::format_to(ctx.out(), "[{:032b}]", mask.get_value());
            } else if constexpr (sizeof(jbx::Component_Mask) == 8) { // 64 bits
                return fmt::format_to(ctx.out(), "[{:064b}]", mask.get_value());
            } else {
                ERROR_IF(true, "Formatter not implemented for { Component_Mask } of this capacity!");
            }
	    }

	};

#endif
