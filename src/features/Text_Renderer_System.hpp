
#pragma once
#include <ecs/ecs.hpp>

namespace jbx {

    /*
        Draw text of the given font, color and position.
    */
    class Text_Renderer_System final : public Base_System {
    public:
        Text_Renderer_System();
        ~Text_Renderer_System() = default;

        void
        update(f64 dt) override;
    };

} // jbx