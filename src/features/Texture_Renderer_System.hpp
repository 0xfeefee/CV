
#pragma once
#include <ecs/ecs.hpp>
#include <2d_engine/2d_engine_api.hpp>

namespace jbx {

    /*
        Draws a texture, or a subtexture for the given entity.
    */
    class Texture_Renderer_System final : public Base_System {
    public:
        Texture_Renderer_System();
        ~Texture_Renderer_System() = default;

        void
        update(f64 delta_time) override;
    };

} // jbx
