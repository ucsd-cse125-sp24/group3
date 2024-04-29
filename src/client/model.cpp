#include "client/model.hpp"

#include <exception>
#include <ostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>
#include <optional>
#include <iostream>

#include "assimp/material.h"
#include "glm/ext/matrix_transform.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>


Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures) : 
    vertices(vertices), indices(indices), textures(textures) {

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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::cout << "Loaded mesh with " << vertices.size() << " vertices, and " << textures.size() << " textures" << std::endl;
}

void Mesh::Draw(std::shared_ptr<Shader> shader, glm::mat4 modelView) const {
    // actiavte the shader program
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

    auto color = glm::vec3(0.0f, 1.0f, 1.0f);

    // get the locations and send the uniforms to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "viewProj"), 1, false, reinterpret_cast<float*>(&viewProjMtx));
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "model"), 1, GL_FALSE, reinterpret_cast<float*>(&modelView));
    glUniform3fv(glGetUniformLocation(shader->getID(), "DiffuseColor"), 1, &color[0]);

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

        shader->setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].getID());
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(VAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

Model::Model(const std::string& filepath) : 
    modelView(1.0f) {

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_SplitLargeMeshes | aiProcess_OptimizeMeshes);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::invalid_argument(std::string("ERROR::ASSIMP::") + importer.GetErrorString());
    }

    processNode(scene->mRootNode, scene);
}

void Model::Draw(std::shared_ptr<Shader> shader) {
    for(const Mesh& mesh : this->meshes)
        mesh.Draw(shader, this->modelView);
}

void Model::TranslateTo(const glm::vec3 &new_pos) {
    modelView[3] = glm::vec4(new_pos, 1.0f);
}

void Model::Scale(const float& new_factor) {
    glm::vec3 scaleVector(new_factor, new_factor, new_factor);
    this->modelView = glm::scale(this->modelView, scaleVector);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(processMesh(mesh, scene));			
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
        glm::vec3 normal(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z);
        
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
    // process indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process material
    if(mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}  


std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, const aiTextureType& type) {
    std::vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture(std::string(str.C_Str()), type);
        textures.push_back(texture);
    }
    return textures;
}


Texture::Texture(const std::string& filepath, const aiTextureType& type) {
    switch (type) {
        case aiTextureType_DIFFUSE:
            this->type = "texture_diffuse";
        case aiTextureType_SPECULAR:
            this->type = "texture_specular";
        default:
            throw std::invalid_argument(std::string("Unimplemented texture type ") + aiTextureTypeToString(type));
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    std::cout << "attempting to load texture at " << filepath << std::endl;
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrComponents, 0);
    if (!data) {
        std::cout << "Texture failed to load at path: " << filepath << std::endl;
        stbi_image_free(data);
    }
    GLenum format;
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


