#pragma once
/*
    Every frontend must implement the following hooks.
*/
namespace leonard {

    void
    frontend_start();

    void
    frontend_step(f64 delta_time);

    void
    frontend_stop();

}
