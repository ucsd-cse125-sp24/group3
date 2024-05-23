#include "client/model.hpp"

#include <exception>
#include <ostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <sys/types.h>
#include <vector>
#include <optional>
#include <iostream>
#include <filesystem>

#include "client/lightsource.hpp"
#include "client/renderable.hpp"
#include "client/constants.hpp"
#include "client/util.hpp"
#include "server/game/torchlight.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/utilities/constants.hpp"

#include "assimp/types.h"
#include "assimp/aabb.h"
#include "assimp/material.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "glm/ext/matrix_transform.hpp"

Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices,
    const std::vector<Texture>& textures,
    const Material& material) : 
    vertices(vertices), indices(indices), textures(textures), material(material) {

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
    
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));

    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, textureCoords)));

    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tangent)));
    
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, bitangent)));
   
    // vertex bone ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, m_boneIDs)));

    // vertex bone weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, m_weights)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // for (int i = 0; i < vertices.size(); i++) {
    //     auto v = vertices[i];
    //     for (int j = 0; j < 4; j++) {
    //         if (v.m_boneIDs[j] >= 0) {
    //             std::cout << "v[" << i << "]: " << v.m_boneIDs[j] << ", " << v.m_weights[j] << std::endl;
    //         }
    //     }
    // }

    // std::cout << "Loaded mesh with " << vertices.size() << " vertices, and " << textures.size() << " textures" << std::endl;
    // std::cout << "\t diffuse " << glm::to_string(this->material.diffuse) << std::endl;
    // std::cout << "\t ambient " << glm::to_string(this->material.diffuse) << std::endl;
    // std::cout << "\t specular " << glm::to_string(this->material.specular) << std::endl;
    // std::cout << "\t shininess" << this->material.shininess << std::endl;
}

void Mesh::draw(
    std::shared_ptr<Shader> shader,
    glm::mat4 viewProj,
    glm::vec3 camPos,
    std::array<boost::optional<SharedObject>, MAX_POINT_LIGHTS> lightSources,
    bool fill) {
    // activate the shader program
    shader->use();

    // vertex shader uniforms
    shader->setMat4("viewProj", viewProj);
    auto model = this->getModelMat();
    
    shader->setMat4("model", model);

    // fragment shader uniforms
    shader->setVec3("material.diffuse", this->material.diffuse);
    shader->setVec3("material.ambient", this->material.ambient);
    shader->setVec3("material.specular", this->material.specular);
    shader->setFloat("material.shininess", this->material.shininess);

    shader->setVec3("viewPos", camPos);

    // set lightsource uniforms 
    unsigned int curr_light_num = 0;
    for (auto curr_source : lightSources) {
        if (curr_light_num > MAX_POINT_LIGHTS) {
            break;
        }
        if (!curr_source.has_value()) {
            continue;
        }

        SharedPointLightInfo& properties = curr_source->pointLightInfo.value();
        glm::vec3 pos = curr_source->physics.getCenterPosition();

        std::string pointLight = "pointLights[" + std::to_string(curr_light_num) + "]";
        shader->setBool(pointLight + ".enabled", true);
        shader->setFloat(pointLight + ".intensity", properties.intensity);
        shader->setVec3(pointLight + ".position", pos);
        // needed for attenuation
        shader->setFloat(pointLight + ".constant", 1.0f);
        shader->setFloat(pointLight + ".linear", properties.attenuation_linear);
        shader->setFloat(pointLight + ".quadratic", properties.attenuation_quadratic);

        // light color
        shader->setVec3(pointLight + ".ambient", properties.ambient_color);
        shader->setVec3(pointLight + ".diffuse", properties.diffuse_color);
        shader->setVec3(pointLight + ".specular", properties.specular_color);

        curr_light_num++;
    }

    if (textures.size() == 0) {
    } else {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        for(unsigned int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = textures[i].getType();
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++);

            std::string shaderTextureName = "material." + name + number;
            shader->setInt(shaderTextureName, i);
            glBindTexture(GL_TEXTURE_2D, textures[i].getID());
        }
        glActiveTexture(GL_TEXTURE0);
    }

    // draw mesh
    glBindVertexArray(VAO);
    if(fill){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

Model::Model(const std::string& filepath) {
    this->directory = std::filesystem::path(filepath).parent_path().string();

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filepath, 
            aiProcess_Triangulate | // flag to only creates geometry made of triangles
            aiProcess_FlipUVs | 
            aiProcess_SplitLargeMeshes | 
            aiProcess_OptimizeMeshes | 
            aiProcess_GenBoundingBoxes); // needed to query bounding box of the model later
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::invalid_argument(std::string("ERROR::ASSIMP::") + importer.GetErrorString());
    }

    processNode(scene->mRootNode, scene);
    std::cout << "Loaded model from " << filepath << std::endl;
    std::cout << "\tDimensions: " << glm::to_string(this->getDimensions()) << std::endl;
}

void Model::draw(std::shared_ptr<Shader> shader,
    glm::mat4 viewProj,
    glm::vec3 camPos, 
    std::array<boost::optional<SharedObject>, MAX_POINT_LIGHTS> lightSources,
    bool fill) {

    for(Mesh& mesh : this->meshes) {
        mesh.draw(shader, viewProj, camPos, lightSources, fill);
    }
}

void Model::translateAbsolute(const glm::vec3& new_pos) {
    Renderable::translateAbsolute(new_pos);
    for(Mesh& mesh : this->meshes) {
        mesh.translateAbsolute(new_pos);
    }
}

void Model::translateRelative(const glm::vec3& delta) {
    Renderable::translateRelative(delta);
    for(Mesh& mesh : this->meshes) {
        mesh.translateAbsolute(delta);
    }
}

void Model::scaleAbsolute(const float& new_factor) {
    Renderable::scaleAbsolute(new_factor);
    for(Mesh& mesh : this->meshes) {
        mesh.scaleAbsolute(new_factor);
    }
}

void Model::scaleAbsolute(const glm::vec3& scale) {
    Renderable::scaleAbsolute(scale);
    for(Mesh& mesh : this->meshes) {
        mesh.scaleAbsolute(scale);
    }
}

void Model::scaleRelative(const float& new_factor) {
    Renderable::scaleRelative(new_factor);
    for(Mesh& mesh : this->meshes) {
        mesh.scaleRelative(new_factor);
    }
}

void Model::scaleRelative(const glm::vec3& scale) {
    Renderable::scaleRelative(scale);
    for(Mesh& mesh : this->meshes) {
        mesh.scaleRelative(scale);
    }
}

void Model::rotateAbsolute(const glm::vec3& dir, const glm::vec3& axis) {
    Renderable::rotateAbsolute(dir, axis);
    for(Mesh& mesh : this->meshes) {
        mesh.rotateAbsolute(dir, axis);
    }
}

void Model::rotateRelative(const glm::vec3& dir, const glm::vec3& axis) {
    Renderable::rotateRelative(dir, axis);
    for(Mesh& mesh : this->meshes) {
        mesh.rotateRelative(dir, axis);
    }
}

void Model::clear() {
    Renderable::clear();
    for(Mesh& mesh : this->meshes) {
        mesh.clear();
    }
}

void Model::clearScale() {
    Renderable::clearScale();
    for(Mesh& mesh : this->meshes) {
        mesh.clearScale();
    }
}

void Model::clearPosition() {
    Renderable::clearScale();
    for(Mesh& mesh : this->meshes) {
        mesh.clearPosition();
    }
}

glm::vec3 Model::getDimensions() {
    return this->dimensions;
}

void Model::setDimensions(const glm::vec3& dimensions) {
    auto scaleFactor = dimensions / this->dimensions;
    this->scaleAbsolute(scaleFactor);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        // std::cout << "processing mesh at index[" << i << "]" << std::endl;
        meshes.push_back(processMesh(mesh, scene));			

        // update model's bounding box with new mesh
        const Bbox meshBbox = aiBboxToGLM(mesh->mAABB);
        this->bbox = combineBboxes(this->bbox, meshBbox);
        // update dimensions based on updated bbox
        this->dimensions = this->bbox.getDimensions();
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // process vertex positions, normals and texture coordinates
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        glm::vec3 position(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z);
        // std::cout << "mNumVertices: " << mesh->mNumVertices << std::endl;
        // std::cout << i << std::endl;

        glm::vec3 normal = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
        if (mesh->mNormals) {
            // std::cout << "Process vertex[" << i << "]: " << mesh->mNormals[i].x << ", ";
            // std::cout << mesh->mNormals[i].y << ", ";
            // std::cout << mesh->mNormals[i].z << std::endl;
            normal = glm::vec3(
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z);

        }

        // check if the mesh contain texture coordinates
        glm::vec2 texture(0.0f, 0.0f);
        if(mesh->mTextureCoords[0]) {
            texture.x = mesh->mTextureCoords[0][i].x; 
            texture.y = mesh->mTextureCoords[0][i].y;
        }

        vertices.push_back(Vertex{
            position,
            normal,
            texture
        });
    }

    for (int i = 0; i < vertices.size(); i++) {
        setDefaultVertexBoneData(vertices[i]);
    }

    // process indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process material
    aiColor3D diffuse_color;
    aiColor3D ambient_color;
    aiColor3D specular_color;
    float shininess = 0.0f;

    if(mesh->mMaterialIndex >= 0) {
        // std::cout << "processing material of id: " << mesh->mMaterialIndex << std::endl;
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        if(AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color)) {
            std::cout << "couldn't get diffuse color" << std::endl;
        }

        if(AI_SUCCESS != material->Get(AI_MATKEY_COLOR_AMBIENT, ambient_color)) {
            std::cout << "couldn't get ambient color" << std::endl;
        }

        if(AI_SUCCESS != material->Get(AI_MATKEY_COLOR_SPECULAR, specular_color)) {
            std::cout << "couldn't get specular color" << std::endl;
        }

        if(AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, shininess)) {
            std::cout << "couldn't get shininess factor" << std::endl;
        }
    }

    extractBoneWeight(vertices, mesh, scene);

    // std::cout << "vertex bone data" << std::endl;

    // for (int i = 0; i < vertices.size(); i++) {
    //     auto v = vertices[i];
    //     for (int j = 0; j < 4; j++) {
    //         if (v.m_boneIDs[j] >= 0) {
    //             std::cout << "v[" << i << "]: " << v.m_boneIDs[j] << ", " << v.m_weights[j] << std::endl;
    //         }
    //     }
    // }

    return Mesh(
        vertices,
        indices,
        textures,
        Material {
            aiColorToGLM(diffuse_color),
            aiColorToGLM(ambient_color),
            aiColorToGLM(specular_color),
            shininess
        }
    );
}  

void Model::setDefaultVertexBoneData(Vertex& vertex) {
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        vertex.m_boneIDs[i] = -1;
        vertex.m_weights[i] = 0.0f;
    }
}

void Model::setVertexBoneData(Vertex& vertex, int boneID, float weight) {
    // std::cout << "setting vertex bone data" << std::endl;
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if (vertex.m_boneIDs[i] < 0) {
            vertex.m_boneIDs[i] = boneID;
            vertex.m_weights[i] = weight;
            return;
        }
    }
}

void Model::extractBoneWeight(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
    auto& boneInfoMap = m_boneInfoMap;
    int& boneCount = m_boneCounter;

    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCount;
            newBoneInfo.offset = matrixToGLM(mesh->mBones[boneIndex]->mOffsetMatrix);
            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCount;
            boneCount++;
        }
        else
        {
            boneID = boneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            setVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, const aiTextureType& type) {
    std::vector<Texture> textures;
    // std::cout << "material has " << mat->GetTextureCount(type) << " textures of type " << aiTextureTypeToString(type) << std::endl;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::filesystem::path textureFilepath = std::filesystem::path(this->directory) / std::string(str.C_Str());
        Texture texture(textureFilepath.string(), type);
        textures.push_back(texture);
    }
    return textures;
}

Texture::Texture(const std::string& filepath, const aiTextureType& type) {
    switch (type) {
        case aiTextureType_DIFFUSE:
            this->type = "texture_diffuse";
            break;
        case aiTextureType_SPECULAR:
            this->type = "texture_specular";
            break;
        default:
            throw std::invalid_argument(std::string("Unimplemented texture type ") + aiTextureTypeToString(type));
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    // std::cout << "Attempting to load texture at " << filepath << std::endl;
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrComponents, 0);
    if (!data) {
        std::cout << "Texture failed to load at path: " << filepath << std::endl;
        stbi_image_free(data);
        throw std::exception();
    }
    // std::cout << "Succesfully loaded texture at " << filepath << std::endl;
    GLenum format = GL_RED;
    if (nrComponents == 1)
        format = GL_RED;
    else if (nrComponents == 3)
        format = GL_RGB;
    else if (nrComponents == 4)
        format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    this->ID = textureID;
}

unsigned int Texture::getID() const {
    return ID;
}

std::string Texture::getType() const {
    return type;
}


