#pragma once

#include <memory>
#include <vector>
#include <set>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "client/shader.hpp"
#include "client/util.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/utilities/constants.hpp"



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
    virtual void draw(Shader* shader,
            glm::mat4 viewProj,
            glm::vec3 camPos, 
            std::array<boost::optional<SharedObject>, MAX_POINT_LIGHTS> lightSources,
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
     * @brief Rotates the item along the specified axis. If
     * no axis is specified, then assumes a rotation on the
     * y-axis. This will not stack upon previous rotations.
     * 
     * @param angle The angle of rotation
     * @param axis The axis of rotation 
     */
    virtual void rotateAbsolute(const glm::vec3& dir, const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f));

    /**
     * @brief Rotates the item along the specified axis. If
     * no axis is specified, then assumes a rotation on the
     * y-axis. This will stack upon previous rotations.
     * 
     * @param angle The angle of rotation
     * @param axis The axis of rotation 
     */
    virtual void rotateRelative(const glm::vec3& dir, const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f));

    /**
     * @brief Rotates the item along the specified axis. If
     * no axis is specified, then assumes a rotation on the
     * y-axis. This will not stack upon previous rotations.
     * 
     * @param angle The angle of rotation
     * @param axis The axis of rotation 
     */
    virtual void rotateAbsolute(const glm::vec3& dir, const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f));

    /**
     * @brief Rotates the item along the specified axis. If
     * no axis is specified, then assumes a rotation on the
     * y-axis. This will stack upon previous rotations.
     * 
     * @param angle The angle of rotation
     * @param axis The axis of rotation 
     */
    virtual void rotateRelative(const glm::vec3& dir, const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f));


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
    glm::quat rotation;
};
