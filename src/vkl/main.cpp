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
 * # Subpages
 * - \subpage createWindowPage "Creating a window"
 *
 * # Note pages
 * - \subpage notePage1 "Testing Note Page"
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
 * Creating a window introduction
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