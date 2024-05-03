#include "client/lightsource.hpp"

#include <memory>

#include <glm/glm.hpp>

#include "client/shader.hpp"

LightSource::LightSource() {
    this->lightPos = glm::vec3(1.0f, 10.0f, 0.0f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the vertex attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void LightSource::draw(std::shared_ptr<Shader> shader) {
    shader->use();
    // Currently 'hardcoding' camera logic in
    float FOV = 45.0f;
    float Aspect = 1.33f;
    float NearClip = 0.1f;
    float FarClip = 100.0f;

    float Distance = 10.0f;
    float Azimuth = 0.0f;
    float Incline = 20.0f;

    glm::mat4 world(1);
    world[3][2] = Distance;
    world = glm::eulerAngleY(glm::radians(-Azimuth)) * glm::eulerAngleX(glm::radians(-Incline)) * world;

    // Compute view matrix (inverse of world matrix)
    glm::mat4 view = glm::inverse(world);

    // Compute perspective projection matrix
    glm::mat4 project = glm::perspective(glm::radians(FOV), Aspect, NearClip, FarClip);

    // Compute final view-projection matrix
    glm::mat4 viewProjMtx = project * view;

    // get the locations and send the uniforms to the shader
    shader->setMat4("viewProj", viewProjMtx);
    shader->setMat4("model", model);

    // draw the light cube object
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glUseProgram(0);
}
