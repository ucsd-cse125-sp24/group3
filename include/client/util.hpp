#pragma once

#include <stdio.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// #include <GL/glew.h>

// #include <GLFW/glfw3.h>
// #include <glm/glm.hpp>

#include "client/core.hpp"

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
