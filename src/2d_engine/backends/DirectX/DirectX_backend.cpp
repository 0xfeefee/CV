
// Implements:
#include <2d_engine/backend_hook.hpp>

// Dependencies:
#include <2d_engine/2d_engine.hpp>


namespace cv {

    void
    initialize_and_start_backend() {
        // Get the config:
        Unique<Engine_Config>& config = get_context<Engine_Config>();

        ERROR_IF(42, "DirectX is the meaning of life!");
    }

} // cv
