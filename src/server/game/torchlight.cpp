
#include "server/game/torchlight.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

SharedObject Torchlight::toShared() {
    auto so = Object::toShared();
    return so;
}

Torchlight::Torchlight() : Object(ObjectType::Torchlight) {
}

Torchlight::~Torchlight() {}
