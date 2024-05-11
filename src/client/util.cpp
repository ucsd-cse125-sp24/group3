#include "client/util.hpp"

#include <algorithm>
#include <limits>
#include <iostream>

#include "glm/fwd.hpp"
#include "glm/glm.hpp"

glm::vec3 aiColorToGLM(const aiColor3D& color) {
    return glm::vec3(color.r, color.g, color.b);
}

glm::vec3 Bbox::getDimensions() {
    return glm::abs(this->corners.first - this->corners.second);
}

Bbox aiBboxToGLM(const aiAABB& bbox) {
    return Bbox {
        .corners = std::pair<glm::vec3, glm::vec3>(
            glm::vec3(
                bbox.mMin.x,
                bbox.mMin.y,
                bbox.mMin.z
            ),
            glm::vec3(
                bbox.mMax.x,
                bbox.mMax.y,
                bbox.mMax.z
            )
        )
    };
}

Bbox combineBboxes(const Bbox& bbox1, const Bbox& bbox2) {
    float minX = std::numeric_limits<float>::infinity();
    float minY = std::numeric_limits<float>::infinity();
    float minZ = std::numeric_limits<float>::infinity(); 
    float maxX = -std::numeric_limits<float>::infinity();
    float maxY = -std::numeric_limits<float>::infinity();
    float maxZ = -std::numeric_limits<float>::infinity(); 

    minX = std::min(minX, bbox1.corners.first.x);
    minX = std::min(minX, bbox1.corners.second.x);
    minX = std::min(minX, bbox2.corners.first.x);
    minX = std::min(minX, bbox2.corners.second.x);

    minY = std::min(minY, bbox1.corners.first.y);
    minY = std::min(minY, bbox1.corners.second.y);
    minY = std::min(minY, bbox2.corners.first.y);
    minY = std::min(minY, bbox2.corners.second.y);

    minZ = std::min(minZ, bbox1.corners.first.z);
    minZ = std::min(minZ, bbox1.corners.second.z);
    minZ = std::min(minZ, bbox2.corners.first.z);
    minZ = std::min(minZ, bbox2.corners.second.z);

    maxX = std::max(maxX, bbox1.corners.first.x);
    maxX = std::max(maxX, bbox1.corners.second.x);
    maxX = std::max(maxX, bbox2.corners.first.x);
    maxX = std::max(maxX, bbox2.corners.second.x);

    maxY = std::max(maxY, bbox1.corners.first.y);
    maxY = std::max(maxY, bbox1.corners.second.y);
    maxY = std::max(maxY, bbox2.corners.first.y);
    maxY = std::max(maxY, bbox2.corners.second.y);

    maxZ = std::max(maxZ, bbox1.corners.first.z);
    maxZ = std::max(maxZ, bbox1.corners.second.z);
    maxZ = std::max(maxZ, bbox2.corners.first.z);
    maxZ = std::max(maxZ, bbox2.corners.second.z);

    return Bbox{
        .corners = std::pair<glm::vec3, glm::vec3>(
            glm::vec3(minX, minY, minZ),
            glm::vec3(maxX, maxY, maxZ)
        )
    };
}
