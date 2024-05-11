#include "server/game/item.hpp"
#include "shared/game/sharedobject.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/objectmanager.hpp"

/*  Constructors and Destructors    */
Item::Item(ObjectType type) :
    Object(type), iteminfo(SharedItemInfo{ .held = false, .used = false }) 
{
	this->setModel(ModelType::Cube);
    this->physics.movable = false;
}

void Item::doCollision(Object* other, ServerGameState* state) {

	auto player = dynamic_cast<Player*>(other);
	if (player == nullptr) return; // only allow players to pick up items

	if (player->inventory.size() < MAX_ITEMS) {
		for (int x : {1, 2, 3, 4}) {
			if (!player->inventory.contains(x)) {
				player->inventory[x] = this->typeID;
				player->sharedInventory.inventory[x] = this->modelType;
				break;
			}
		}
		this->iteminfo.held = true;		
		this->physics.collider = Collider::None;
	}
}

/*	SharedGameState generation	*/
SharedObject Item::toShared() {
    auto so = Object::toShared();
    so.iteminfo = this->iteminfo;
    return so;
}