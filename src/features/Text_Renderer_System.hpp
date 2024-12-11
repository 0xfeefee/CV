
#pragma once
#include <ecs/ecs.hpp>
#include <2d_engine/2d_engine_api.hpp>

namespace jbx {

    class Text_Renderer_System final : public Base_System {
    public:
        Text_Renderer_System() {
            component_mask.add<Text>();
            component_mask.add<Rect>();
        }

        void
        update(f64 dt) override {
            Unique<Registry>& registry = get_context<Registry>();
            for (auto& entity: entities) {
                Rect& rect = registry->get_component<Rect>(entity);
                Text& text = registry->get_component<Text>(entity);

                draw_text(text, {rect.x, rect.y});
            }
        }
    };

} // jbx