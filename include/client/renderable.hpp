#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "client/shader.hpp"

class Renderable {
 public:
     Renderable();
    /**
     * Draws the renderable item 
     *
     * @param Shader to use while drawing all the
     * meshes of the model
     * @param
     */
    virtual void draw(std::shared_ptr<Shader> shader,
            glm::mat4 viewProj,
            glm::vec3 camPos, 
            glm::vec3 lightPos,
            bool fill) = 0;

    /**
     * Sets the position of the item to the given x,y,z
     * values
     *
     * @param vector of x, y, z of the 's new position
     */
    void translateAbsolute(const glm::vec3& new_pos);

    /**
     * Updates the position of the item relative to it's
     * previous position
     *
     * @param vector of x, y, z of the change in the item's
     * position
     */
    void translateRelative(const glm::vec3& delta);

    /**
     * Scale the Model across all axes (x,y,z)
     * by a factor
     *
     * @param new_factor describes how much to scale the model by.
     * Ex: setting it to 0.5 will cut the model's rendered size  
     * in half.
     */
    void scale(const float& new_factor);

    /**
     * Scale the item across all axes (x,y,z)
     * by the scale factor in each axis.
     *
     * @param the scale vector describes how much to independently scale 
     * the item in each axis (x, y, z)
     */
    void scale(const glm::vec3& scale);

    /**
     * Gets the model matrix given all the transformations 
     * applied to it
     *
     * @return updated model matrix
     */
    glm::mat4 getModelMat();
 private:
    glm::mat4 model;
};