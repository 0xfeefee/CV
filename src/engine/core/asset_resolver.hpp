

/*
    Resources intentionally only support 1 extension, in order to simplify and reduce scope further...
*/
std::string
image_path(const std::string& file_name) {
    return get_context<Engine_Config>()->root_dir + "assets/images/" + file_name + ".png";
}

std::string
sound_path(const std::string& file_name) {
    return get_context<Engine_Config>()->root_dir + "assets/sounds/" + file_name + ".wav";
}

std::string
font_path(const std::string& file_name) {
    return get_context<Engine_Config>()->root_dir + "assets/fonts/" + file_name + ".ttf";
}

std::string
model_path(const std::string& file_name) {
    return get_context<Engine_Config>()->root_dir + "assets/models/" + file_name + ".obj";
}

std::string
model_path_glb(const std::string& file_name) {
    return get_context<Engine_Config>()->root_dir + "assets/models_glb/" + file_name + ".glb";
}