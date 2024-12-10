
#pragma once

#include <ecs/Component.hpp>
#include <ecs/Entity.hpp>

namespace jbx {

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

} // jbx
