
#pragma once
/*
    { Component_Mask } is used to determine which components an entity has, or which components
    a system requires.

    Bit size of the { value } member determines how many different components types can exist at
    once. In order to increase the number of component types we can handle, simply change the
    type of the value to that of a different size.
*/
namespace jbx {

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
        const u8
        get_value() const;

        /*
            Sets the value to 0, effectively removing all the components from the mask.
        */
        void
        reset();

        /*
            Check if given { Component_Mask } is fully expressed in the current mask.
        */
        bool
        contains(const Component_Mask& other) const;
    };

} // jbx
