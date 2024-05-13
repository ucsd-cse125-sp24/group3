
#include "server/game/torchlight.hpp"
#include "glm/fwd.hpp"
#include "server/game/collider.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

SharedObject Torchlight::toShared() {
    auto so = Object::toShared();
    return so;
}

Torchlight::Torchlight(glm::vec3 corner):
	Object(ObjectType::Torchlight, Physics(false, 
		Collider::Box, corner, glm::vec3(0.0f), glm::vec3(1.0f)),
		ModelType::Torchlight)
{
}

Torchlight::~Torchlight() {}
