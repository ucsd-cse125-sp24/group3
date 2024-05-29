#pragma once

#include <vector>
#include <utility>
#include <optional>

#include <glm/glm.hpp>
#include "shared/game/celltype.hpp"
#include "shared/utilities/typedefs.hpp"
#include "client/gui/img/img.hpp"

namespace gui {
    class GUI;
}

#define VISIBLE_WIDTH 21

class Minimap {
public:
    Minimap();

    void addToGUI(gui::GUI* gui);

    void loadMap(const std::vector<std::vector<CellType>>& map); 

    void setSelfId(EntityID eid);

    void updatePlayerPosition(EntityID id, glm::ivec2 pos);

private:
    std::vector<std::vector<CellType>> map;
    EntityID self_eid;

    std::vector<std::pair<EntityID, glm::ivec2>> player_positions;

    using VisibleMap = std::array<std::array<gui::img::ImgID, VISIBLE_WIDTH>, VISIBLE_WIDTH>;

    VisibleMap getVisibleMap(glm::ivec2 center) const;
};
