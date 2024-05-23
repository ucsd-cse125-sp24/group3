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

/**
 * @brief Class representing a camera for rendering. Primarily used to compute the 
 * view-projection matrix and pass it to the Client for rendering objects from
 * different viewing angles.
 */
class Camera {

public:
    /**
     * @brief Constructs a new Camera object.
     * 
     */
    Camera();

    /**
     * @brief Destroys the Camera object.
     * 
     */
    ~Camera();

    // Access functions
    /**
     * @brief Sets the aspect ratio of the camera.
     * 
     * @param a The new aspect ratio.
     */
    void setAspect(float a) { aspect = a; }

    /**
     * @brief Updates the the camera based on the input screen coordinates.
     * 
     * @param xpos New x-coordinate for the cursor.
     * @param ypos New y-coordinate for the cursor.
     */
    virtual void update(float xpos, float ypos);

    /**
     * @brief Moves the camera based on the axis, direction, and speed. Factors in the current 
     * direction of the camera to compute proper lateral and forward movement.
     * 
     * @param is_x_axis A flag denoting whether the movement is along the x-axis or z-axis.
     * @param dir The direction that the camera is moving in.
     * @return glm::vec3 An output vector representing the movement of the camera in the specified
     * axis and direction.
     */
    glm::vec3 move(bool is_x_axis, float dir);

    /**
     * @brief Returns the view-projection matrix of the Camera, for use in rendering.
     * 
     * @return glm::mat4 The view-projection matrix.
     */
    glm::mat4 getViewProj() { return viewProjMat; }

    /**
     * @brief Returns a normalized vector representing the direction that the camera is facing.
     * 
     * @return glm::vec3 A normalized vector representing the direction the camera is facing.
     */
    glm::vec3 getFacing() { return cameraFront; }

    /**
     * @brief Updates the position of the camera.
     * 
     * @param pos The new position of the camera.
     */
    void updatePos(glm::vec3 pos);

    glm::vec3 getPos();

    glm::mat4 getProjection();

    glm::mat4 getView();

protected:
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
    glm::vec3 cameraRight;
    glm::vec3 worldUp;

    float speed;

    glm::mat4 viewProjMat;

    glm::mat4 projection;
    glm::mat4 view;
};

class DungeonMasterCamera : public Camera {
public:
    DungeonMasterCamera();
    ~DungeonMasterCamera();

    void update(float xpos, float ypos) override;
};

