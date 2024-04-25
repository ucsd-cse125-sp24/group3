#pragma once

#include "client/core.hpp"

#include <iostream>
#include <ostream>
#include <utility>
#include <unordered_map>

#include "client/util.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

class Camera {

public:
    Camera();
    ~Camera();

    // Access functions
    void setAspect(float a) { aspect = a; }

    void update(float xpos, float ypos);
    void move(bool is_x_axis, float dir);

    glm::mat4 getViewProj();

private:
    // Perspective controls
    float FOV;       // Field of View Angle (degrees)
    float aspect;    // Aspect Ratio
    float nearClip;  // Near clipping plane distance
    float farClip;   // Far clipping plane distance

    float yaw;
    float pitch;
    float lastX;
    float lastY;
    float fov;

    bool firstMouse;
    float sensitivity;

    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

    float speed;

    glm::mat4 viewProjMat;
};

