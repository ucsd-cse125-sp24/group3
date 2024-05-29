#include "client/minimap.hpp"
#include "client/gui/gui.hpp"
#include "client/client.hpp"
#include "shared/game/celltype.hpp"
#include <vector>
#include <iostream>

using namespace gui;

Minimap::Minimap() {

}

void Minimap::loadMap(const std::vector<std::vector<CellType>>& map) {
    this->map = map;
}

void Minimap::setSelfId(EntityID self_eid) {
    this->self_eid = self_eid;
}

void Minimap::updatePlayerPosition(EntityID id, glm::ivec2 pos) {
    for (auto& [curr_id, curr_pos] : this->player_positions) {
        if (curr_id == id) {
            curr_pos = pos;
            return;
        }
    }

    // haven't inserted this player in here yet
    this->player_positions.push_back({id, pos});
}

struct ivec2_hash {
    size_t operator()(const glm::ivec2& vec) const {
        size_t seed = 0;
        boost::hash_combine(seed, vec.x);
        boost::hash_combine(seed, vec.y);
        return seed;
    }
};

void Minimap::addToGUI(gui::GUI* gui) {
    std::optional<glm::ivec2> center;

    std::unordered_map<glm::ivec2, EntityID, ivec2_hash> pos_to_player;
    for (const auto& [id, pos] : this->player_positions) {
        pos_to_player.insert({pos, id});
        if (id == this->self_eid) {
            center = pos;
        }
    }

    if (!center.has_value()) {
        std::cerr << "WARNING: could not find player id in minimap, skipping map render.\n";
        return;
    }

    auto visible_map = this->getVisibleMap(center.value());

    // now a whole bunch of calculations for rendering to the screen in the top right...

    const float IMG_CELL_SIZE = gui::font::getRelativePixels(24); // each img is 24x24 pixels
    const float MAP_WIDTH_PX = IMG_CELL_SIZE * VISIBLE_WIDTH;

    const float LEFT_X = WINDOW_WIDTH - MAP_WIDTH_PX;
    const float BOTTOM_Y = WINDOW_HEIGHT - MAP_WIDTH_PX;

    for (int i = 0; i < visible_map.size(); i++) {
        int row = visible_map.size() - 1 - i; // start at bottom row
        auto& row_arr = visible_map.at(row);
        auto row_flex = widget::Flexbox::make(
            glm::vec2(LEFT_X, BOTTOM_Y + (IMG_CELL_SIZE * i)),
            glm::vec2(MAP_WIDTH_PX, 0.0f), 
            widget::Flexbox::Options(widget::Dir::HORIZONTAL, widget::Align::LEFT, 0.0f)
        );

        for (int col = 0; col < row_arr.size(); col++) {
            glm::ivec2 coord(col, row);

            img::ImgID img = visible_map.at(row).at(col);
            if (pos_to_player.contains(coord)) {
                img = img::ImgID::MazePlayer1; // TODO: change based on the eid
            }

            row_flex->push(widget::StaticImg::make(gui->imageLoader().getImg(img)));
        }

        gui->addWidget(std::move(row_flex));
    }
}

Minimap::VisibleMap Minimap::getVisibleMap(glm::ivec2 center) const {
    Minimap::VisibleMap visible_map;     
    
    glm::ivec2 top_left = center - (VISIBLE_WIDTH / 2);
    for (int i = 0; i < VISIBLE_WIDTH; i++) {
        int row = top_left.y + i;
        for (int j = 0; j < VISIBLE_WIDTH; j++) {
            int col = top_left.x + j;
            if (row < 0 || row >= map.size() || col < 0 || col >= map.at(row).size()) {
                visible_map[i][j] = gui::img::ImgID::MazeBlank;
            } else {
                visible_map[i][j] = gui::img::cellTypeToImage(map[row][col]);
            }
        }
    }

    return visible_map;
}
