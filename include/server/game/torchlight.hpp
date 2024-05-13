#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "shared/game/sharedobject.hpp"

class Torchlight : public Object {
public:
	/**
	 * @param corner Corner position of the surface
	 */
	Torchlight(glm::vec3 corner);
	~Torchlight();

	virtual SharedObject toShared() override;
private:

};
