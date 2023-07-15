#pragma once

#include <cstdlib>

extern Nasha::Application* Nasha::createApplication();

int main(int argc, char** argv){
    auto app = Nasha::createApplication();
    std::cout << "Successfully created application" << std::endl;
    try {
        app -> run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        delete app;
        return EXIT_FAILURE;
    }

    delete app;
    return EXIT_SUCCESS;
}
