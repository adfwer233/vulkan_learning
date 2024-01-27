#include "application.hpp"
#include "iostream"
/**
 * \mainpage Learning Vulkan Project
 *
 * # Introduction
 *
 * The aim of this project is learning vulkan programming, from graphics pipeline to parallel programming using compute
 * shader.
 *
 * # Current Features
 * - Creating a window
 * - Physical Devices and Queue Families
 *
 * # Note pages
 * - \subpage createWindowPage "Creating a window"
 * - \subpage devicePage "Vulkan Device Introduction"
 */

/**
 * \page notePage1 Note Page1
 *
 * This is note page
 */

/**
 * \page createWindowPage Creating a window
 *
 * To Create a window, we should create a GLFW window and create a vulkan instance.
 *
 * We created two class, `VklWindow` and `VklDevice`. Following standard RAII paradigm, we create GLFW window and vulkan
 * instance in their constructors.
 */

int main() {
    Application app{};

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}