#include "client/camera.hpp"
// Code adapted from https://learnopengl.com/Getting-started/Camera

Camera::Camera() : cameraPos(glm::vec3(0.0f)), cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)), viewProjMat(glm::mat4(1.0f)) {
    FOV = 45.0f;
    aspect = 1.33f;
    nearClip = 0.1f;
    farClip = 100.0f;

    yaw   = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    pitch =  0.0f;
    lastX =  800.0f / 2.0;
    lastY =  600.0 / 2.0;
    fov   =  45.0f;

    sensitivity = 0.1f;
    firstMouse = true;

    speed = 0.125f;
}

Camera::~Camera() {

}

void Camera::update(float xpos, float ypos) {

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    glm::mat4 projection = glm::perspective(glm::radians(FOV), aspect, nearClip, farClip);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    viewProjMat = projection * view;
}

glm::vec3 Camera::move(bool is_x_axis, float dir) {
    if (is_x_axis) {
        glm::vec3 effCameraFront = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
        return dir * speed * effCameraFront;
    } else {
        glm::vec3 effCameraFront = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
        return dir * glm::normalize(glm::cross(effCameraFront, cameraUp)) * speed;
    }
}

void Camera::updatePos(glm::vec3 pos) {
    cameraPos = pos;
}