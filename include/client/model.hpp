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
#include "client/shader.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoords;
};

class Texture {
 public:
    Texture(const std::string& filepath, const aiTextureType& type);
    unsigned int getID() const;
    std::string getType() const;
 private:
    unsigned int ID;
    std::string type;
};

class Mesh {
 public:
     std::vector<Vertex>       vertices;
     // std::vector<unsigned int> indices;
     // std::vector<Texture>      textures;

     std::vector<glm::vec3>       positions;
     std::vector<glm::vec3>       normals;
     std::vector<unsigned int> indices;
     std::vector<Texture>      textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(std::shared_ptr<Shader> shader, glm::mat4 modelView) const;
 private:
    //  render data opengl needs
    GLuint VAO, VBO, EBO;
    // GLuint VAO, VBO_positions, VBO_normals, EBO;
};


class Model {
 public:
    /**
     * Loads Model from a given filename. Can be of format
     * .obj, .blend or any of the formats that assimp supports
     * @see https://assimp-docs.readthedocs.io/en/latest/about/introduction.html?highlight=obj#introduction 
     *
     * @param Filepath to model file.
     */
    Model(const std::string& filepath);

    /**
     * Draws all the meshes of a given model
     *
     * @param Shader to use while drawing all the
     * meshes of the model
     */
    void Draw(std::shared_ptr<Shader> shader);	

    void Update(const glm::vec3& new_pos);
    void Scale(const float& new_factor);
 private:
    // model data
    std::vector<Mesh> meshes;

    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, const aiTextureType& type);

    glm::mat4 modelView;
};

// #ifndef MODEL_H
// #define MODEL_H
//
// #include <GL/glew.h>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <stb_image.h>
// #include <assimp/Importer.hpp>
// #include <assimp/scene.h>
// #include <assimp/postprocess.h>
//
//
// #include <string>
// #include <fstream>
// #include <sstream>
// #include <iostream>
// #include <map>
// #include <vector>
// using namespace std;
//
//
// #include <glm/glm.hpp>
//
// #include <string>
// #include <fstream>
// #include <sstream>
// #include <iostream>
//
// class Shader
// {
// public:
//     unsigned int ID;
//     // constructor generates the shader on the fly
//     // ------------------------------------------------------------------------
//     Shader() = default;
//     Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
//     {
//         // 1. retrieve the vertex/fragment source code from filePath
//         std::string vertexCode;
//         std::string fragmentCode;
//         std::string geometryCode;
//         std::ifstream vShaderFile;
//         std::ifstream fShaderFile;
//         std::ifstream gShaderFile;
//         // ensure ifstream objects can throw exceptions:
//         vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//         fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//         gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//         try 
//         {
//             // open files
//             vShaderFile.open(vertexPath);
//             fShaderFile.open(fragmentPath);
//             std::stringstream vShaderStream, fShaderStream;
//             // read file's buffer contents into streams
//             vShaderStream << vShaderFile.rdbuf();
//             fShaderStream << fShaderFile.rdbuf();		
//             // close file handlers
//             vShaderFile.close();
//             fShaderFile.close();
//             // convert stream into string
//             vertexCode = vShaderStream.str();
//             fragmentCode = fShaderStream.str();			
//             // if geometry shader path is present, also load a geometry shader
//             if(geometryPath != nullptr)
//             {
//                 gShaderFile.open(geometryPath);
//                 std::stringstream gShaderStream;
//                 gShaderStream << gShaderFile.rdbuf();
//                 gShaderFile.close();
//                 geometryCode = gShaderStream.str();
//             }
//         }
//         catch (std::ifstream::failure& e)
//         {
//             std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
//         }
//         const char* vShaderCode = vertexCode.c_str();
//         const char * fShaderCode = fragmentCode.c_str();
//         // 2. compile shaders
//         unsigned int vertex, fragment;
//         // vertex shader
//         vertex = glCreateShader(GL_VERTEX_SHADER);
//         glShaderSource(vertex, 1, &vShaderCode, NULL);
//         glCompileShader(vertex);
//         checkCompileErrors(vertex, "VERTEX");
//         // fragment Shader
//         fragment = glCreateShader(GL_FRAGMENT_SHADER);
//         glShaderSource(fragment, 1, &fShaderCode, NULL);
//         glCompileShader(fragment);
//         checkCompileErrors(fragment, "FRAGMENT");
//         // if geometry shader is given, compile geometry shader
//         unsigned int geometry;
//         if(geometryPath != nullptr)
//         {
//             const char * gShaderCode = geometryCode.c_str();
//             geometry = glCreateShader(GL_GEOMETRY_SHADER);
//             glShaderSource(geometry, 1, &gShaderCode, NULL);
//             glCompileShader(geometry);
//             checkCompileErrors(geometry, "GEOMETRY");
//         }
//         // shader Program
//         ID = glCreateProgram();
//         glAttachShader(ID, vertex);
//         glAttachShader(ID, fragment);
//         if(geometryPath != nullptr)
//             glAttachShader(ID, geometry);
//         glLinkProgram(ID);
//         checkCompileErrors(ID, "PROGRAM");
//         // delete the shaders as they're linked into our program now and no longer necessary
//         glDeleteShader(vertex);
//         glDeleteShader(fragment);
//         if(geometryPath != nullptr)
//             glDeleteShader(geometry);
//
//     }
//     // activate the shader
//     // ------------------------------------------------------------------------
//     void use() 
//     { 
//         glUseProgram(ID); 
//     }
//     // utility uniform functions
//     // ------------------------------------------------------------------------
//     void setBool(const std::string &name, bool value) const
//     {         
//         glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
//     }
//     // ------------------------------------------------------------------------
//     void setInt(const std::string &name, int value) const
//     { 
//         glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
//     }
//     // ------------------------------------------------------------------------
//     void setFloat(const std::string &name, float value) const
//     { 
//         glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
//     }
//     // ------------------------------------------------------------------------
//     void setVec2(const std::string &name, const glm::vec2 &value) const
//     { 
//         glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
//     }
//     void setVec2(const std::string &name, float x, float y) const
//     { 
//         glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
//     }
//     // ------------------------------------------------------------------------
//     void setVec3(const std::string &name, const glm::vec3 &value) const
//     { 
//         glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
//     }
//     void setVec3(const std::string &name, float x, float y, float z) const
//     { 
//         glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
//     }
//     // ------------------------------------------------------------------------
//     void setVec4(const std::string &name, const glm::vec4 &value) const
//     { 
//         glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
//     }
//     void setVec4(const std::string &name, float x, float y, float z, float w) 
//     { 
//         glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
//     }
//     // ------------------------------------------------------------------------
//     void setMat2(const std::string &name, const glm::mat2 &mat) const
//     {
//         glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
//     }
//     // ------------------------------------------------------------------------
//     void setMat3(const std::string &name, const glm::mat3 &mat) const
//     {
//         glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
//     }
//     // ------------------------------------------------------------------------
//     void setMat4(const std::string &name, const glm::mat4 &mat) const
//     {
//         glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
//     }
//
// private:
//     // utility function for checking shader compilation/linking errors.
//     // ------------------------------------------------------------------------
//     void checkCompileErrors(GLuint shader, std::string type)
//     {
//         GLint success;
//         GLchar infoLog[1024];
//         if(type != "PROGRAM")
//         {
//             glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
//             if(!success)
//             {
//                 glGetShaderInfoLog(shader, 1024, NULL, infoLog);
//                 std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
//             }
//         }
//         else
//         {
//             glGetProgramiv(shader, GL_LINK_STATUS, &success);
//             if(!success)
//             {
//                 glGetProgramInfoLog(shader, 1024, NULL, infoLog);
//                 std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
//             }
//         }
//     }
// };
//
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
//
//
// #include <string>
// #include <vector>
// using namespace std;
//
// #define MAX_BONE_INFLUENCE 4
//
// struct Vertex {
//     // position
//     glm::vec3 Position;
//     // normal
//     glm::vec3 Normal;
//     // texCoords
//     glm::vec2 TexCoords;
//     // tangent
//     glm::vec3 Tangent;
//     // bitangent
//     glm::vec3 Bitangent;
// 	//bone indexes which will influence this vertex
// 	int m_BoneIDs[MAX_BONE_INFLUENCE];
// 	//weights from each bone
// 	float m_Weights[MAX_BONE_INFLUENCE];
// };
//
// struct Texture {
//     unsigned int id;
//     string type;
//     string path;
// };
//
// class Mesh {
// public:
//     // mesh Data
//     vector<Vertex>       vertices;
//     vector<unsigned int> indices;
//     vector<Texture>      textures;
//     unsigned int VAO;
//
//     // constructor
//     Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
//     {
//         this->vertices = vertices;
//         this->indices = indices;
//         this->textures = textures;
//
//         // now that we have all the required data, set the vertex buffers and its attribute pointers.
//         setupMesh();
//     }
//
//     // render the mesh
//     void Draw(Shader &shader) 
//     {
//         // bind appropriate textures
//         unsigned int diffuseNr  = 1;
//         unsigned int specularNr = 1;
//         unsigned int normalNr   = 1;
//         unsigned int heightNr   = 1;
//         for(unsigned int i = 0; i < textures.size(); i++)
//         {
//             glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
//             // retrieve texture number (the N in diffuse_textureN)
//             string number;
//             string name = textures[i].type;
//             if(name == "texture_diffuse")
//                 number = std::to_string(diffuseNr++);
//             else if(name == "texture_specular")
//                 number = std::to_string(specularNr++); // transfer unsigned int to string
//             else if(name == "texture_normal")
//                 number = std::to_string(normalNr++); // transfer unsigned int to string
//              else if(name == "texture_height")
//                 number = std::to_string(heightNr++); // transfer unsigned int to string
//
//             // now set the sampler to the correct texture unit
//             glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
//             // and finally bind the texture
//             glBindTexture(GL_TEXTURE_2D, textures[i].id);
//         }
//         
//         // draw mesh
//         glBindVertexArray(VAO);
//         glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
//         glBindVertexArray(0);
//
//         // always good practice to set everything back to defaults once configured.
//         glActiveTexture(GL_TEXTURE0);
//     }
//
// private:
//     // render data 
//     unsigned int VBO, EBO;
//
//     // initializes all the buffer objects/arrays
//     void setupMesh()
//     {
//         // create buffers/arrays
//         glGenVertexArrays(1, &VAO);
//         glGenBuffers(1, &VBO);
//         glGenBuffers(1, &EBO);
//
//         glBindVertexArray(VAO);
//         // load data into vertex buffers
//         glBindBuffer(GL_ARRAY_BUFFER, VBO);
//         // A great thing about structs is that their memory layout is sequential for all its items.
//         // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
//         // again translates to 3/2 floats which translates to a byte array.
//         glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  
//
//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//         glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
//
//         // set the vertex attribute pointers
//         // vertex Positions
//         glEnableVertexAttribArray(0);	
//         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
//         // vertex normals
//         glEnableVertexAttribArray(1);	
//         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
//         // vertex texture coords
//         glEnableVertexAttribArray(2);	
//         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
//         // vertex tangent
//         glEnableVertexAttribArray(3);
//         glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
//         // vertex bitangent
//         glEnableVertexAttribArray(4);
//         glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
// 		// ids
// 		glEnableVertexAttribArray(5);
// 		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
//
// 		// weights
// 		glEnableVertexAttribArray(6);
// 		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
//         glBindVertexArray(0);
//     }
// };
//
// class Model 
// {
// public:
//     // model data 
//     vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
//     vector<Mesh>    meshes;
//     string directory;
//     bool gammaCorrection;
//
//     // constructor, expects a filepath to a 3D model.
//     Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
//     {
//         loadModel(path);
//     }
//
//     // draws the model, and thus all its meshes
//     void Draw(Shader &shader)
//     {
//         for(unsigned int i = 0; i < meshes.size(); i++)
//             meshes[i].Draw(shader);
//     }
//     
// private:
//     // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
//     void loadModel(string const &path)
//     {
//         // read file via ASSIMP
//         Assimp::Importer importer;
//         const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
//         // check for errors
//         if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
//         {
//             cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
//             return;
//         }
//         // retrieve the directory path of the filepath
//         directory = path.substr(0, path.find_last_of('/'));
//
//         // process ASSIMP's root node recursively
//         processNode(scene->mRootNode, scene);
//     }
//
//     // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
//     void processNode(aiNode *node, const aiScene *scene)
//     {
//         // process each mesh located at the current node
//         for(unsigned int i = 0; i < node->mNumMeshes; i++)
//         {
//             // the node object only contains indices to index the actual objects in the scene. 
//             // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
//             aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//             meshes.push_back(processMesh(mesh, scene));
//         }
//         // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
//         for(unsigned int i = 0; i < node->mNumChildren; i++)
//         {
//             processNode(node->mChildren[i], scene);
//         }
//
//     }
//
//     Mesh processMesh(aiMesh *mesh, const aiScene *scene)
//     {
//         // data to fill
//         vector<Vertex> vertices;
//         vector<unsigned int> indices;
//         vector<Texture> textures;
//
//         // walk through each of the mesh's vertices
//         for(unsigned int i = 0; i < mesh->mNumVertices; i++)
//         {
//             Vertex vertex;
//             glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
//             // positions
//             vector.x = mesh->mVertices[i].x;
//             vector.y = mesh->mVertices[i].y;
//             vector.z = mesh->mVertices[i].z;
//             vertex.Position = vector;
//             // normals
//             if (mesh->HasNormals())
//             {
//                 vector.x = mesh->mNormals[i].x;
//                 vector.y = mesh->mNormals[i].y;
//                 vector.z = mesh->mNormals[i].z;
//                 vertex.Normal = vector;
//             }
//             // texture coordinates
//             if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
//             {
//                 glm::vec2 vec;
//                 // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
//                 // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
//                 vec.x = mesh->mTextureCoords[0][i].x; 
//                 vec.y = mesh->mTextureCoords[0][i].y;
//                 vertex.TexCoords = vec;
//                 // tangent
//                 vector.x = mesh->mTangents[i].x;
//                 vector.y = mesh->mTangents[i].y;
//                 vector.z = mesh->mTangents[i].z;
//                 vertex.Tangent = vector;
//                 // bitangent
//                 vector.x = mesh->mBitangents[i].x;
//                 vector.y = mesh->mBitangents[i].y;
//                 vector.z = mesh->mBitangents[i].z;
//                 vertex.Bitangent = vector;
//             }
//             else
//                 vertex.TexCoords = glm::vec2(0.0f, 0.0f);
//
//             vertices.push_back(vertex);
//         }
//         // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
//         for(unsigned int i = 0; i < mesh->mNumFaces; i++)
//         {
//             aiFace face = mesh->mFaces[i];
//             // retrieve all indices of the face and store them in the indices vector
//             for(unsigned int j = 0; j < face.mNumIndices; j++)
//                 indices.push_back(face.mIndices[j]);        
//         }
//         // process materials
//         aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
//         // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
//         // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
//         // Same applies to other texture as the following list summarizes:
//         // diffuse: texture_diffuseN
//         // specular: texture_specularN
//         // normal: texture_normalN
//
//         // 1. diffuse maps
//         vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
//         textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
//         // 2. specular maps
//         vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
//         textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
//         // 3. normal maps
//         std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
//         textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
//         // 4. height maps
//         std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
//         textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
//         
//         // return a mesh object created from the extracted mesh data
//         return Mesh(vertices, indices, textures);
//     }
//
//     // checks all material textures of a given type and loads the textures if they're not loaded yet.
//     // the required info is returned as a Texture struct.
//     vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
//     {
//         vector<Texture> textures;
//         for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
//         {
//             aiString str;
//             mat->GetTexture(type, i, &str);
//             // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
//             bool skip = false;
//             for(unsigned int j = 0; j < textures_loaded.size(); j++)
//             {
//                 if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
//                 {
//                     textures.push_back(textures_loaded[j]);
//                     skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
//                     break;
//                 }
//             }
//             if(!skip)
//             {   // if texture hasn't been loaded already, load it
//                 Texture texture;
//                 // texture.id = TextureFromFile(str.C_Str(), this->directory);
//                 texture.type = typeName;
//                 texture.path = str.C_Str();
//                 textures.push_back(texture);
//                 textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
//             }
//         }
//         return textures;
//     }
// };
//
//
// #endif
