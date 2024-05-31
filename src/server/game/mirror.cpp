#include "server/game/mirror.hpp"
#include "server/game/constants.hpp"

Mirror::Mirror(glm::vec3 corner, glm::vec3 dimensions)
	: Item(ObjectType::Mirror, false, corner, ModelType::Cube, dimensions) {
	this->modelType = ModelType::Mirror;

}

void Mirror::useItem(Object* other, ServerGameState& state, int itemSelected) {
	//	Get Player object that used the mirror
	this->used_player = state.objects.getPlayer(other->typeID);

	//	Get use time
	this->used_time = std::chrono::system_clock::now();

	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds{ this->used_time - now };
	this->iteminfo.remaining_time = (double)(MIRROR_USE_DURATION) - elapsed_seconds.count();

	//	Set used to true (since mirror is being used)
	this->iteminfo.used = true;

	//	Add mirror to used items map
	this->used_player->sharedInventory.usedItems.insert(
		{ this->typeID,
		{
			ModelType::Mirror,
			this->iteminfo.remaining_time
		}
	});

	//	Don't call Item::useItem() to avoid consuming the mirror
}

void Mirror::dropItem(Object* other, ServerGameState& state, int itemSelected, float dropDistance) {
	//	Stop mirror's effect
	this->revertEffect(state);

	Item::dropItem(other, state, itemSelected, dropDistance);
}

bool Mirror::timeOut() {
	//	Determine whether mirror use has timed out
	auto now = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds{ now - this->used_time };

	//	Update remaining time
	this->iteminfo.remaining_time =
		(double)MIRROR_USE_DURATION - elapsed_seconds.count();

	//	Update used items' remaining time
	if (this->used_player->sharedInventory.usedItems.find(this->typeID)
		!= this->used_player->sharedInventory.usedItems.end()) {
		this->used_player->sharedInventory.usedItems[this->typeID].second
			= this->iteminfo.remaining_time;
	}

	//	Return true if duration has elapsed
	return elapsed_seconds.count() > MIRROR_USE_DURATION;
}

UsedItemsMap::iterator Mirror::revertEffect(ServerGameState& state) {
	//	Mak as no longer used (but still held)
	this->iteminfo.used = false;

	//	Update used items list to mark mirror as no longer used
	auto it = this->used_player->sharedInventory.usedItems.find(this->typeID);

	if (it != this->used_player->sharedInventory.usedItems.end()) {
		it = this->used_player->sharedInventory.usedItems.erase(it);
	}

	return it;
}