#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <optional>
#include <set>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "assimp/material.h"

#include "client/renderable.hpp"
#include "client/shader.hpp"
#include "client/util.hpp"
#include "client/bone.hpp"

#define MAX_BONE_INFLUENCE 4

/**
 * Stores position, normal vector, and coordinates 
 * in texture map for every vertex in a mesh.
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    int m_boneIDs[MAX_BONE_INFLUENCE]; /* Bone indices which influence the vertex */
    float m_weights[MAX_BONE_INFLUENCE]; /* Weights for each bone */
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

struct BoneInfo {
	/*id is index in finalBoneMatrices*/
	int id;

	/*offset matrix transforms vertex from model space to bone space*/
	glm::mat4 offset;
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
            std::array<boost::optional<SharedObject>, MAX_POINT_LIGHTS> lightSources,
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
            std::array<boost::optional<SharedObject>, MAX_POINT_LIGHTS> lightSources,
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
     * by a factor. This will not stack up on top of any 
     * previous scaling.
     *
     * @param new_factor describes how much to scale the model by.
     * Ex: setting it to 0.5 will cut the model's rendered size  
     * in half.
     */
    virtual void scaleAbsolute(const float& new_factor) override;

    /**
     * Scale the item across all axes (x,y,z)
     * by the scale factor in each axis. This will not stack 
     * up on top of any previous scaling.
     *
     * @param the scale vector describes how much to independently scale 
     * the item in each axis (x, y, z)
     */
    virtual void scaleAbsolute(const glm::vec3& scale) override;

    /**
     * Scale the Model across all axes (x,y,z)
     * by a factor. This will stack 
     * up on top of any previous scaling.
     *
     * @param new_factor describes how much to scale the model by.
     * Ex: setting it to 0.5 will cut the model's rendered size  
     * in half.
     */
    void scaleRelative(const float& new_factor) override;

    /**
     * Scale the item across all axes (x,y,z)
     * by the scale factor in each axis. This 
     * will stack up on top of any previous scaling.
     *
     * @param the scale vector describes how much to independently scale 
     * the model in each axis (x, y, z)
     */
    void scaleRelative(const glm::vec3& scale) override;

    /**
     * @brief Rotates the item along the specified axis. If
     * no axis is specified, then assumes a rotation on the
     * y-axis. This will not stack upon previous rotations.
     * 
     * @param angle The angle of rotation
     * @param axis The axis of rotation 
     */
    void rotateAbsolute(const glm::vec3& dir, const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f)) override;

    /**
     * @brief Rotates the item along the specified axis. If
     * no axis is specified, then assumes a rotation on the
     * y-axis. This will stack upon previous rotations.
     * 
     * @param angle The angle of rotation
     * @param axis The axis of rotation 
     */
    void rotateRelative(const glm::vec3& dir, const glm::vec3& axis = glm::vec3(0.0f, 1.0f, 0.0f)) override;


    /**
     * Clear transformations and reset the model matrix 
     * to the identity.
     */
    void clear() override;

    /**
     * Reset scale factors in each dimension to 1.0
     */
    void clearScale() override;

    /**
     * Reset translation to position (0, 0, 0)
     */
    void clearPosition() override;

    /**
     * Queries the dimensions of a bounding box around 
     * the model.
     *
     * @return a vec3 where each dimension represents the size
     * in that dimension
     */
    glm::vec3 getDimensions();

    /**
     * Set dimensions by scaling the model accordingly
     *
     * @param a vec3 where each dimension represents the size
     * in that dimension
     */
    void setDimensions(const glm::vec3& dimensions);

	auto& getBoneInfoMap() { return m_boneInfoMap; }
	int& getBoneCount() { return m_boneCounter; }
	

 private:
    std::vector<Mesh> meshes;
    Bbox bbox;
    glm::vec3 dimensions;
    
	std::map<std::string, BoneInfo> m_boneInfoMap;
	int m_boneCounter = 0;


    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	void setDefaultVertexBoneData(Vertex& vertex);
	void setVertexBoneData(Vertex& vertex, int boneID, float weight);
	void extractBoneWeight(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, const aiTextureType& type);


    // store the directory of the model file so that textures can be
    // loaded relative to the model file
    std::string directory;
};
