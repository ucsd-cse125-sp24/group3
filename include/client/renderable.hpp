#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "client/shader.hpp"
#include "shared/game/sharedobject.hpp"

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
            std::vector<SharedObject> lightSources,
            bool fill) = 0;

    /**
     * Sets the position of the item to the given x,y,z
     * values
     *
     * @param vector of x, y, z of the 's new position
     */
    virtual void translateAbsolute(const glm::vec3& new_pos);

    /**
     * Updates the position of the item relative to it's
     * previous position
     *
     * @param vector of x, y, z of the change in the item's
     * position
     */
    virtual void translateRelative(const glm::vec3& delta);

    /**
     * Scale the Model across all axes (x,y,z)
     * by a factor. This will not stack up on top of any 
     * previous scaling.
     *
     * @param new_factor describes how much to scale the model by.
     * Ex: setting it to 0.5 will cut the model's rendered size  
     * in half.
     */
    virtual void scaleAbsolute(const float& new_factor);

    /**
     * Scale the item across all axes (x,y,z)
     * by the scale factor in each axis. This will not stack 
     * up on top of any previous scaling.
     *
     * @param the scale vector describes how much to independently scale 
     * the item in each axis (x, y, z)
     */
    virtual void scaleAbsolute(const glm::vec3& scale);

    /**
     * Scale the Model across all axes (x,y,z)
     * by a factor. This will stack 
     * up on top of any previous scaling.
     *
     * @param new_factor describes how much to scale the model by.
     * Ex: setting it to 0.5 will cut the model's rendered size  
     * in half.
     */
    virtual void scaleRelative(const float& new_factor);

    /**
     * Scale the item across all axes (x,y,z)
     * by the scale factor in each axis. This 
     * will stack up on top of any previous scaling.
     *
     * @param the scale vector describes how much to independently scale 
     * the item in each axis (x, y, z)
     */
    virtual void scaleRelative(const glm::vec3& scale);

    /**
     * Gets the model matrix given all the transformations 
     * applied to it
     *
     * @return updated model matrix
     */
    glm::mat4 getModelMat();

    /**
     * Clear transformations and reset the model matrix 
     * to the identity.
     */
    virtual void clear();

    /**
     * Reset scale factors in each dimension to 1.0
     */
    virtual void clearScale();

    /**
     * Reset translation to position (0, 0, 0)
     */
    virtual void clearPosition();
 private:
    glm::mat4 model;
};
