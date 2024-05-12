#include "server/game/item.hpp"
#include "shared/game/sharedobject.hpp"

/*  Constructors and Destructors    */
// TODO: actually make the item constructor take params
// and fill in these values (probably on ted's branch, hi ted!)
Item::Item():
    Object(ObjectType::Item, Physics(false, Collider::Box, glm::vec3(0.0f), glm::vec3(0.0f)), ModelType::Cube) { // cppcheck-suppress uninitMemberVar
}

Item::~Item() {

}

/*	SharedGameState generation	*/
SharedObject Item::toShared() {
    auto so = Object::toShared();
    so.iteminfo = this->iteminfo;
    return so;
}