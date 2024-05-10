#include "client/util.hpp"

#include <algorithm>

#include "glm/glm.hpp"

glm::vec3 aiColorToGLM(const aiColor3D& color) {
    return glm::vec3(color.r, color.g, color.b);
}

glm::vec3 Bbox::getDimensions() {
    return this->max - this->min;
}

bool operator==(const Bbox& bbox1, const Bbox& bbox2) {
    return (bbox1.min == bbox2.min) && (bbox2.max == bbox2.max);
}

Bbox aiBboxToGLM(const aiAABB& bbox) {
    return Bbox {
        .min = glm::vec3(
            bbox.mMin.x,
            bbox.mMin.y,
            bbox.mMin.z
        ),
        .max = glm::vec3(
            bbox.mMax.x,
            bbox.mMax.y,
            bbox.mMax.z
        ),
    };
}

Bbox combineBboxes(const Bbox& bbox1, const Bbox& bbox2) {
    glm::vec3 newMin(
        std::min(bbox1.min.x, bbox2.min.x),
        std::min(bbox1.min.y, bbox2.min.y),
        std::min(bbox1.min.z, bbox2.min.z));
    glm::vec3 newMax(
        std::max(bbox1.max.x, bbox2.max.x),
        std::max(bbox1.max.y, bbox2.max.y),
        std::max(bbox1.max.z, bbox2.max.z));
    return Bbox{
        newMin,
        newMax
    };
}
