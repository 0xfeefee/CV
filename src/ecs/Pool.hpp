
#pragma once

namespace jbx {

    /*
        Just an interface which enables us to use the Pool with different types of components
        later.
    */
    class Base_Pool {
    public:
        virtual ~Base_Pool() {}

        virtual void
        remove_entity_from_pool(int entity_id) = 0;
    };

    /*
        { Pool } is just an abstraction over vector that we use in the { Registry } to store the
        data. @todo: Simplify, reduce access time.
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

} // jbx
