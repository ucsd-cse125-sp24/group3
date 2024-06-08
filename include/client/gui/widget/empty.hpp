#pragma once

#include "client/gui/widget/widget.hpp"

namespace gui::widget {
class Empty : public Widget {
public:
	using Ptr = std::unique_ptr<Empty>;

	template <typename... Params>
	static Ptr make(Params&&... params) {
		return std::make_unique<Empty>(std::forward<Params>(params)...);
	}

	Empty(glm::vec2 origin, glm::vec2 size);
	explicit Empty(glm::vec2 size) : Empty(glm::vec2(0, 0), size) {}
	Empty(float width, float height) : Empty(glm::vec2(width, height)) {}
	explicit Empty(float width) : Empty(width, 0.0f) {}

	void render() override;
private:
};
}