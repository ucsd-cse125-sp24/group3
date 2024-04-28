#pragma once

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
  

class Shader {
 public:
    // constructor reads and builds the shader
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    unsigned int getID();
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;
 private:
    // the shader program ID
    unsigned int ID;
};
