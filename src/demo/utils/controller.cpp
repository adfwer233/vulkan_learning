#include "demo/utils/controller.hpp"

#include "ray_tracer/ray_picker.hpp"

#include "vkl/scene/vkl_scene.hpp"

#include <format>
#include <iostream>


KeyboardCameraController* KeyboardCameraController::instance_ = nullptr;

void KeyboardCameraController::setCamera(Camera &t_camera) {
    camera = &t_camera;
}

void KeyboardCameraController::processInput(GLFWwindow *window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    auto callProcessKeyboard = [&](CameraMovement direction, float deltaTime) {
        actionCallBack();
        camera->process_keyboard(direction, deltaTime);
    };

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        pressing_shift = true;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        pressing_shift = false;
}

void KeyboardCameraController::scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
    auto controller = instance_;
    controller->camera->process_mouse_scroll(y_offset);
}

void KeyboardCameraController::mouse_button_callback(GLFWwindow *window, int button, int state, int mod) {
    auto controller = instance_;

    if (not controller->in_region)
        return;

    if (button == GLFW_MOUSE_BUTTON_MIDDLE and state == GLFW_PRESS) {
        controller->is_mouse_pressing = true;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT and state == GLFW_PRESS) {
        controller->is_mouse_pressing = true;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT and state == GLFW_PRESS) {
        controller->uiManager_->pickObject(controller->mouse_x_pos, controller->mouse_y_pos);
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE and state == GLFW_RELEASE) {
        controller->actionCallBack();
        controller->mouse_flag = true;
        controller->is_mouse_pressing = false;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT and state == GLFW_RELEASE) {
        controller->actionCallBack();
        controller->mouse_flag = true;
        controller->is_mouse_pressing = false;
    }
}

void KeyboardCameraController::mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    auto controller = instance_;

    controller->mouse_x_pos = static_cast<float>(xposIn);
    controller->mouse_y_pos = static_cast<float>(yposIn);

    if (xposIn > 1024 or yposIn > 1024)
        controller->in_region = false;
    else
        controller->in_region = true;

    if (not controller->is_mouse_pressing)
        return;

    if (controller->mouse_flag) {
        controller->last_x = controller->mouse_x_pos;
        controller->last_y = controller->mouse_y_pos;
        controller->mouse_flag = false;
    }

    float x_offset = controller->mouse_x_pos - controller->last_x;
    float y_offset = controller->last_y - controller->mouse_y_pos; // reversed since y-coordinates go from bottom to top

    controller->last_x = controller->mouse_x_pos;
    controller->last_y = controller->mouse_y_pos;

    if (controller->pressing_shift)
        controller->camera->process_mouse_shift_movement(x_offset, y_offset);
    else
        controller->camera->process_mouse_movement(x_offset, y_offset);
}

void KeyboardCameraController::set_scene(VklScene &scene) {
    scene_ = scene;
}

void KeyboardCameraController::setUIManager(UIManager &uiManager) {
    uiManager_ = &uiManager;
}
