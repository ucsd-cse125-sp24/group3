#include "client/gui/widget/empty.hpp"

namespace gui::widget {
Empty::Empty(glm::vec2 origin, glm::vec2 size) : Widget(Type::Empty, origin) {
	this->width = (size.x > 0.0f) ? size.x : 0.0f;
	this->height = (size.y > 0.0f) ? size.y : 0.0f;
}

void Empty::render() {
	//	Do nothing
}
}