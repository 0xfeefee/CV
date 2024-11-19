
#pragma once

namespace cv {

    /*
        Component_Mask is used to determine which components an entity has, or which components a system
        requires.
    */
    typedef   u16 Component_Mask;
    constexpr int MAX_COMPONENTS = sizeof(Component_Mask) * 8;


    /*
        Entity is just an id. We can maybe do something with tagging too?
    */
    struct Entity {
        int id;
    };


    /*
        Just an interface to make it possible to automatically generate unique id's for every component type.

        For this we use a dirty trick in the subclass - Component, every generated version will increment the
        { next_id } value by 1. One caveat here is that we cannot guarantee that a component type will have
        the same ID every time, but that does not matter for our use case.
    */
    struct IComponent {
    protected:
        static inline int next_id = 0;
    };

    template <typename T>
    class Component: public IComponent {
    public:
        static int
        get_id() {
            static const int id = next_id++;
            ERROR_IF(id > MAX_COMPONENTS, "Maximum component count exceeded!");

            return id;
        }
    };

}