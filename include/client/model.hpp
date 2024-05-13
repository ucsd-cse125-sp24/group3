#pragma once

#include <vector>
#include <string>
#include <memory>
#include <optional>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "assimp/material.h"
#include "client/renderable.hpp"
#include "client/shader.hpp"

/**
 * Stores position, normal vector, and coordinates 
 * in texture map for every vertex in a mesh.
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoords;
};

class Texture {
 public:
    /**
     * Load a texture from a filepath. 
     *
     * @param filepath is the file path to the texture
     * @param type specifies the type of texture to load.
     * Currently only aiTextureType_SPECULAR and aiTextureType_DIFFUSE
     * are implemented.
     */
    Texture(const std::string& filepath, const aiTextureType& type);

    /**
     * @return the texture's ID to be passed into OpenGL functions
     */
    GLuint getID() const;

    /*
     * Get the type of texture. Either "texture_diffuse" or "texture_specular".
     */
    std::string getType() const;
 private:
    GLuint ID;
    std::string type;
};

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

/**
 * Mesh holds the data needed to render a mesh (collection of triangles).
 *
 * A Mesh differs from a Model since a Model is typically made up of multiple,
 * smaller meshes. This is useful for animating parts of model individual (ex: legs,
 * arms, head)
 */
class Mesh : public Renderable {
 public:
    /**
     * Creates a new mesh from a collection of vertices, indices and textures
     */
    Mesh(
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        const std::vector<Texture>& textures,
        const Material& material
    );

    /**
     * Render the Mesh to the viewport using the provided shader program. 
     *
     * @param shader to use when rendering the program. Determines position of
     * vertices and their color/texture.
     * @param modelView determines the scaling/rotation/translation of the 
     * mesh
     */
    void draw(std::shared_ptr<Shader> shader,
            glm::mat4 viewProj,
            glm::vec3 camPos, 
            std::vector<Light> lightSources,
            bool fill) override;
 private:
     std::vector<Vertex>       vertices;
     std::vector<unsigned int> indices;
     std::vector<Texture>      textures;
     Material material;

    //  render data opengl needs
    GLuint VAO, VBO, EBO;
};


class Model : public Renderable {
 public:
    /**
     * Loads Model from a given filename. Can be of format
     * .obj, .blend or any of the formats that assimp supports
     * @see https://assimp-docs.readthedocs.io/en/latest/about/introduction.html?highlight=obj#introduction 
     *
     * @param Filepath to model file.
     */
    explicit Model(const std::string& filepath);

    /**
     * Draws all the meshes of a given model
     *
     * @param Shader to use while drawing all the
     * meshes of the model
     */
    void draw(std::shared_ptr<Shader> shader,
            glm::mat4 viewProj,
            glm::vec3 camPos, 
            std::vector<Light> lightSources,
            bool fill) override;

    /**
     * Sets the position of the Model to the given x,y,z
     * values
     *
     * @param vector of x, y, z of the model's new position
     */
    void translateAbsolute(const glm::vec3& new_pos) override;

    /**
     * Updates the position of the Model relative to it's
     * previous position
     *
     * @param vector of x, y, z of the change in the Model's
     * position
     */
    void translateRelative(const glm::vec3& delta) override;

    /**
     * Scale the Model across all axes (x,y,z)
     * by a factor
     *
     * @param new_factor describes how much to scale the model by.
     * Ex: setting it to 0.5 will cut the model's rendered size  
     * in half.
     */
    void scale(const float& new_factor) override;

    /**
     * Scale the model across all axes (x,y,z)
     * by the scale factor in each axis.
     *
     * @param the scale vector describes how much to independently scale 
     * the model in each axis (x, y, z)
     */
    void scale(const glm::vec3& scale) override;
 private:
    std::vector<Mesh> meshes;

    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, const aiTextureType& type);

    // store the directory of the model file so that textures can be
    // loaded relative to the model file
    std::string directory;
};
