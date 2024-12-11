
#pragma once
#include <ecs/ecs.hpp>
#include <2d_engine/2d_engine_api.hpp>

namespace jbx {

    /*
        Draws a texture, or a subtexture for the given entity.
    */
    class Texture_Renderer_System final : public Base_System {
    public:
        Texture_Renderer_System() {
            component_mask.add<Rect>();
            component_mask.add<Texture>();
        }

        void
        update(f64 delta_time) override {
            Unique<Registry>& registry = get_context<Registry>();
            for (auto& entity: entities) {
                Rect& rect       = registry->get_component<Rect>(entity);
                Texture& texture = registry->get_component<Texture>(entity);

                draw_texture(texture, rect);
            }
        }
    };

} // jbx
