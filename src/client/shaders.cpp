#include <GL/glew.h>
#include <boost/dll/runtime_symbol_info.hpp>

#include "client/util.hpp"

GLuint loadCubeShaders() {
    boost::filesystem::path root_path = boost::dll::program_location().parent_path().parent_path().parent_path();
    boost::filesystem::path vertFilepath = root_path / "src/client/shaders/shader.vert";
    boost::filesystem::path fragFilepath = root_path / "src/client/shaders/shader.frag";

    GLuint shaderProgram = LoadShaders(vertFilepath.c_str(), fragFilepath.c_str());
    // Check the shader program exists and is non-zero
    if (!shaderProgram) {
        return 0;
    }
    return shaderProgram;
}
