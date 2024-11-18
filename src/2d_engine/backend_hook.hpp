
#pragma once

/*
    Every backend must implement the { initialize_and_start_backend } hook, called by { initialize_and_start }.
*/
namespace cv {

    void
    initialize_and_start_backend();

} // cv
