#include "client/gui/widget/empty.hpp"

namespace gui::widget {
Empty::Empty(glm::vec2 origin, glm::vec2 size) : Widget(Type::Empty, origin) {
	this->width = size.x;
	this->height = size.y;
}

void Empty::render() {
	//	Do nothing
}
}