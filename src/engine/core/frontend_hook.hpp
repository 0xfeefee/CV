#pragma once
/*
    Every frontend must implement the following hooks.
*/
namespace jbx {

    /*
        Setup the virtual machine for a given backend and run the user code to initialize their game.
        @todo: should we separate frontend setup - i.e: starting the VM from user lifecycle "start" call?
    */
    void
    frontend_start();

    /*
        Run user code for every single frame.
    */
    void
    frontend_step(f64 delta_time);

    /*
        Run user cleanup code and shut the VM down, or cleanup, ...
    */
    void
    frontend_stop();

    void
    tester();

} // jbx
