
#pragma once
#include <ecs/ecs.hpp>

namespace jbx {

    /*
        Draw the entity rectangle (fill).
    */
    class Rect_Renderer_System final : public Base_System {
    public:
        Rect_Renderer_System();
        ~Rect_Renderer_System() = default;

        void
        update(f64 dt) override;
    };

} // jbx
