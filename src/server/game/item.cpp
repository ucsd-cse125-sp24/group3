#include "server/game/item.hpp"
#include "shared/game/sharedobject.hpp"

/*  Constructors and Destructors    */
Item::Item(ObjectType type) :
    Object(type), iteminfo(SharedItemInfo{ .held = false, .used = false }) 
{
    this->physics.movable = false;
}

/*	SharedGameState generation	*/
SharedObject Item::toShared() {
    auto so = Object::toShared();
    so.iteminfo = this->iteminfo;
    return so;
}