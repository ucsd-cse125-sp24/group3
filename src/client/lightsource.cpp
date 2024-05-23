#include "client/lightsource.hpp"

#include <memory>

#include <glm/glm.hpp>

#include "client/shader.hpp"

LightSource::LightSource() : model(1.0f) {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the vertex attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void LightSource::draw(std::shared_ptr<Shader> shader,
    glm::mat4 viewProj) {
    shader->use();

    // get the locations and send the uniforms to the shader
    shader->setMat4("viewProj", viewProj);
    shader->setMat4("model", model);

    // draw the light cube object
    glBindVertexArray(VAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glUseProgram(0);
}

void LightSource::TranslateTo(const glm::vec3 &new_pos) {
    model[3] = glm::vec4(new_pos, 1.0f);
}
