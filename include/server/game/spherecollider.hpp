#pragma once

#include "server/game/collider.hpp"

class SphereCollider : public Collider {
public:

	SphereCollider();
	SphereCollider(glm::vec3 corner, glm::vec3 dimension);
	~SphereCollider();

	bool detectCollision(Collider* otherCollider) override;
	bool resolveCollision(Collider* otherCollider) override;
	Shape getShape() override;

private:

};