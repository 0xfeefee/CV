
#pragma once

namespace jbx {
    /*
        { Component_Mask } is used to determine which components an entity has, or which
        components a system requires.

        Bit size of the { value } member determines how many different components types can exist
        at once. In order to increase the number of component types we can handle, simply change
        the type of the value to that of a different size.
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

    constexpr int MAX_COMPONENT_TYPES = sizeof(Component_Mask) * 8;

    /*
    ## Component_Mask: template definitions
    */

    template <typename T>
    bool
    Component_Mask::has() const {
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
    ## Component: template implementations
    */

    template <typename T>
    int
    Component<T>::get_type_id() {
        static const int id = next_id++;
        ERROR_IF(id > MAX_COMPONENT_TYPES, "Maximum component count exceeded!");

        return id;
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
