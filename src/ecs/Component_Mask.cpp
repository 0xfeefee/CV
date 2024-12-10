
// Implement:
#include <ecs/Component_Mask.hpp>

namespace jbx {

    const u8
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

}
