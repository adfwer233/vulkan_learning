#include <iostream>
#include <format>
#include "vkl_camera.hpp"

/**
 * @brief get view transformation from camera
 * @return test
 */
glm::mat4 Camera::get_view_transformation() const {
    return glm::lookAt(position, position + camera_front, camera_up_axis);
}

glm::mat4 Camera::get_proj_transformation() const {
    return glm::perspective(glm::radians(this->zoom), 1.0f, 0.1f, 100.0f);
}

void Camera::process_mouse_scroll(float offset) {
    zoom -= (float)offset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

void Camera::process_keyboard(CameraMovement direction, float deltaTime) {
    float velocity = move_speed * deltaTime;

    if (direction == FORWARD)
        position += camera_front * velocity;
    if (direction == BACKWARD)
        position -= camera_front * velocity;
    if (direction == LEFT)
        position -= camera_right_axis * velocity;
    if (direction == RIGHT)
        position += camera_right_axis * velocity;
    if (direction == DOWN)
        position += camera_up_axis * velocity;
    if (direction == UP)
        position -= camera_up_axis * velocity;

    std::cout << std::format("{} {} {} {}\n", position.x, position.y, position.z, deltaTime);

    update_camera_vectors();
}

void Camera::update_camera_vectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera_front = glm::normalize(front);

    // also re-calculate the Right and Up vector
    camera_right_axis =
        glm::normalize(glm::cross(camera_front, world_up)); // normalize the vectors, because their length
    // gets closer to 0 the more you look up or
    // down which results in slower movement.
    camera_up_axis = glm::normalize(glm::cross(camera_right_axis, camera_front));
}

void Camera::process_mouse_movement(float x_offset, float y_offset) {
    x_offset *= mouse_sensitivity;
    y_offset *= mouse_sensitivity;

    yaw += x_offset;
    pitch -= y_offset;

    pitch = std::min(pitch, 89.0f);
    pitch = std::max(pitch, -89.0f);

    update_camera_vectors();
}