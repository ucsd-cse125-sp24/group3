#include "server/game/item.hpp"
#include "shared/game/sharedobject.hpp"

SharedObject Item::toShared() {
    auto so = Object::toShared();
    so.iteminfo = this->iteminfo;
    return so;
}