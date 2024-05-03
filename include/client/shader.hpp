#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
  

class Shader {
 public:
     /**
      * Create a shader program from filepaths to a vertex and fragment shader.
      * Use getID() to access the shader program's ID.
      */
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    /*
     * @return the shader program's ID which can be passed into OpenGL functions 
     */
    GLuint getID();

    /*
     * Activate the shader program. Must be called before drawing.  
     * Calls glUseProgram under the hood.
     */
    void use();

    /*
     * Sets a boolean unform variable of the shader program 
     * with the specified value
     * @param name is the name of the uniform variable as written
     * in the shader program
     * @param value is the boolean value to write to that variable
     */
    void setBool(const std::string &name, bool value) const;  

    /*
     * Sets an integer unform variable of the shader program 
     * with the specified value
     * @param name is the name of the uniform variable as written
     * in the shader program
     * @param value is the integer value to write to that variable
     */
    void setInt(const std::string &name, int value) const;   

    /*
     * Sets a float unform variable of the shader program 
     * with the specified value
     * @param name is the name of the uniform variable as written
     * in the shader program
     * @param value is the float value to write to that variable
     */
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, glm::mat4& value);
    void setVec3(const std::string &name, glm::vec3& value);
 private:
    // the shader program ID
    unsigned int ID;
};
