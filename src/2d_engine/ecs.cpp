
// Implements:
#include <2d_engine/ecs.hpp>

namespace cv {

    /*
    ## Component_Mask: implementation
    */

    const u16
    Component_Mask::get_value() const {
        return value;
    }

    void
    Component_Mask::reset() {
        value = 0;
    }

    bool
    Component_Mask::contains(const Component_Mask& other) const {
        return (value & other.value) == other.value;
    }


    /*
    ## Entity: implementation
    */

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


    /*
    ## Registry: implementation
    */

    Entity
    Registry::create_entity() {
        int entity_id;

        // If no free ids are available:
        if (available_ids.empty()) {
            entity_id     = num_entities;
            num_entities += 1;

            // Resize the collection if necessary:
            if (entity_id >= component_masks.size()) {
                component_masks.resize(entity_id + 1);
            }
        }
        else {
            // Otherwise re-use an id:
            entity_id = available_ids.front();
            available_ids.pop_front();
        }

        Entity entity(entity_id);
        new_entities.insert(entity);

        return entity;
    }

    void
    Registry::kill_entity(Entity entity) {
        dead_entities.insert(entity);
    }

    void
    Registry::update() {
        // Add entitites:
        for (auto new_entity: new_entities) {
            // Get the entity component mask:
            const Component_Mask& entity_mask = component_masks[new_entity.id];

            // Add entity to systems whose component signature it contains:
            for (auto& system: systems) {
                if (entity_mask.contains(system.second->get_component_mask())) {
                    system.second->add_entity(new_entity);
                }
            }
        }
        new_entities.clear();

        // Remove entities:
        for (auto dead_entity: dead_entities) {
            // Remove an entity from systems:
            for (auto system: systems) {
                system.second->remove_entity(dead_entity);
            }
            int entity_id = dead_entity.id;

            // Remove the entity from the component pools:
            for (auto pool: component_pools) {
                if (pool != nullptr) {
                    pool->remove_entity_from_pool(entity_id);
                }
            }

            // Reset the signature for this entity:
            component_masks[entity_id].reset();

            // Make the id available again:
            available_ids.push_back(entity_id);

            // Cleanup any groups:
            ungroup_entity(dead_entity);
        }

        dead_entities.clear();
    }

    void
    Registry::group_entity(Entity entity, const std::string& group) {
        // Create a new set if one does not exist already:
        entities_per_group.emplace(group, std::set<Entity>());

        // Maintain both collections:
        entities_per_group[group].emplace(entity);
        groups_per_entity.emplace(entity.id, group);
    }

    bool
    Registry::entity_belongs_to_group(Entity entity, const std::string& group) const {
        if (entities_per_group.find(group) == entities_per_group.end()) {
            return false;
        }

        auto group_entities                   = entities_per_group.at(group);
        return group_entities.find(entity.id) != group_entities.end();
    }

    std::vector<Entity>
    Registry::get_entities_by_group(const std::string& group) const {
        auto& set_of_entities = entities_per_group.at(group);
        return std::vector<Entity>(set_of_entities.begin(), set_of_entities.end());
    }

    void
    Registry::ungroup_entity(Entity entity) {
        // Iterator to the grouped entity:
        auto grouped_entity = groups_per_entity.find(entity.id);
        if (grouped_entity != groups_per_entity.end()) {

            // Iterator to the group:
            auto group = entities_per_group.find(grouped_entity->second);
            if (group != entities_per_group.end()) {
                auto entity_in_group = group->second.find(entity);
                if (entity_in_group != group->second.end()) {
                    group->second.erase(entity_in_group);
                }
            }

            groups_per_entity.erase(grouped_entity);
        }
    }

} // cv
