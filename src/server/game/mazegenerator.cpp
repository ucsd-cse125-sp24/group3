#include "server/game/mazegenerator.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <memory>
#include <unordered_set>
#include <limits>
#include <random>
#include <algorithm>

#include <boost/graph/adjacency_matrix.hpp>
#include <boost/filesystem.hpp>

#include "shared/utilities/root_path.hpp"
#include "server/game/grid.hpp"
#include "server/game/gridcell.hpp"
#include "shared/utilities/rng.hpp"
#include "shared/utilities/config.hpp"


MazeGenerator::MazeGenerator(GameConfig config) {
    this->_next_room_id = 0;

    boost::filesystem::path rooms_dir = getRepoRoot() / "maps" / "rooms";

    boost::filesystem::path dir_10x10 = rooms_dir / "10x10";
    boost::filesystem::path dir_20x20 = rooms_dir / "20x20";
    boost::filesystem::path dir_40x40 = rooms_dir / "40x40";

    for (auto type : ALL_TYPES) {
        this->rooms_by_type.insert({type, std::vector<std::shared_ptr<Room>>()});
    }

    // has to happen after the for loop loading in the type vectors
    if (!config.game.maze.procedural) {
        auto path = getRepoRoot() / config.game.maze.directory / config.game.maze.maze_file;
        this->_loadRoom(path, false);
        return;
    }

    for (const auto & entry : boost::filesystem::directory_iterator(dir_10x10)) {
        this->_loadRoom(entry.path(), true);
    }
    for (const auto & entry : boost::filesystem::directory_iterator(dir_20x20)) {
        this->_loadRoom(entry.path(), true);
    }
    for (const auto & entry : boost::filesystem::directory_iterator(dir_40x40)) {
        this->_loadRoom(entry.path(), true);
    }

}


std::optional<Grid> MazeGenerator::generate() {
    if (this->rooms_by_id.size() == 1) {
        // not procedural because we only generated one potential room, so just return that as the entire maze
        return this->rooms_by_id.at(0)->grid;
    }

    // clear generated code in case this is a second, third attempt
    this->maze.clear();
    std::queue<std::pair<glm::ivec2, RoomEntry>> empty;
    std::swap(this->frontier, empty);
    
    _placeRoom(_pullRoomByType(RoomType::SPAWN), glm::ivec2(0, 0));

    _generatePolicy();

    _num_rooms_placed = 1;
    while (_num_rooms_placed < REQUIRED_NUM_ROOMS) {
        if (this->frontier.empty()) {
            return {};
        }

        const auto& [coord, required_entryway] = this->frontier.front();
        this->frontier.pop();

        if (!_isOpenWorldCoord(coord)) {
            continue; // something took its place in the meantime
        }

        while (true) {
            auto room = _pullRoomByPolicy();

            if ( (room->rclass.entries & required_entryway) == 0) {
                continue; // can't connect, so pull again
            }

            // coord is the adjacent coord to the room that is already in the maze
            // required_entryway is the entrywawy that this new room is required to have
            // so required_entryway of BOTTOM means we are connecting to the TOP of a previously
            // inserted room.

            std::optional<glm::ivec2> origin_coord;
            for (auto curr_origin_coord : _getPossibleOriginCoords(room, required_entryway, coord)) {
                bool okay = true;
                for (auto room_coord : _getRoomCoordsTakenBy(room->rclass.size, curr_origin_coord)) {
                    if (!_isOpenWorldCoord(room_coord)) {
                        okay = false;
                        break;
                    }
                }
                if (okay) {
                    origin_coord = curr_origin_coord;
                    break;
                }
            }

            if (!origin_coord.has_value()) {
                continue;
            }

            _placeRoom(room, origin_coord.value());
            break;
        }

        _num_rooms_placed++;
    }

    std::cout << "Done: created " << _num_rooms_placed << " rooms.\n";

    int min_x = std::numeric_limits<int>::max();
    int min_y = std::numeric_limits<int>::max();
    int max_x = std::numeric_limits<int>::min();
    int max_y = std::numeric_limits<int>::min();

    for (const auto& [coord, room_id] : this->maze) {
        min_x = std::min(coord.x, min_x);
        min_y = std::min(coord.y, min_y);
        max_x = std::max(coord.x, max_x);
        max_y = std::max(coord.y, max_y);
    }

    int num_rows = max_y - min_y + 1;
    int num_cols = max_x - min_x + 1;

    // Convert internal this->maze into final Grid
    std::unordered_set<glm::ivec2> skip;
    Grid output(num_rows * GRID_CELLS_PER_ROOM, num_cols * GRID_CELLS_PER_ROOM);
    // room_coord guaranteed to be top left coord of room because if ivec2_comparator
    for (const auto& [room_coord, room_id] : this->maze) {
        auto& room = this->rooms_by_id.at(room_id);

        if (skip.contains(room_coord)) {
            continue;
        }

        // skip this room in the future since we've already put it in its entirety in output
        for (const auto& coord : _getRoomCoordsTakenBy(room->rclass.size, room_coord)) {
            skip.insert(coord);
        }

        for (int grid_row = 0; grid_row < room->grid.getRows(); grid_row++) {
            for (int grid_col = 0; grid_col < room->grid.getColumns(); grid_col++) {
                CellType type = room->grid.getCell(grid_col, grid_row)->type;

                int world_row = (room_coord.y - min_y) * GRID_CELLS_PER_ROOM + grid_row;
                int world_col = (room_coord.x - min_x) * GRID_CELLS_PER_ROOM + grid_col;

                output.addCell(world_col, world_row, type);
            }
        }
    }

    // fill in empty spaces for printing to text file
    for (int row = 0; row < output.getRows(); row++) {
        for (int col = 0; col < output.getColumns(); col++) {
            if (output.getCell(col, row) == nullptr) {
                output.addCell(col, row, CellType::Wall);
            }

            if (row == 0 || row == output.getRows() - 1 || col == 0 || col == output.getColumns() - 1) {
                output.getCell(col, row)->type = CellType::Wall;
            }
        }
    }

    return output;
}


void MazeGenerator::_loadRoom(boost::filesystem::path path, bool procedural) {
    std::cout << "Loading room " << path << "\n";

	std::ifstream file;
	file.open(path.string(), std::ifstream::in);

	assert(file.is_open());

	int rows, columns;
	//	Character buffer that stores a single line of the input maze file
	//	(extra character for null terminator)
	char buffer[MAX_MAZE_COLUMNS + 1];

	//	Get number of columns
	//	Read the first line and use its length to get the number of columns
	file.getline(buffer, MAX_MAZE_COLUMNS + 1);
	columns = file.gcount();

	//	If end of file isn't reached, file.gcount() includes the newline
	//	character at the end of the first line; remove it from the column
	//	count.
	if (!file.eof()) {
		columns -= 1;
	}

	//	Get number of rows
	//	Rows is at least one due to the above getline() call
	rows = 1;
	while (!file.eof()) {
		file.getline(buffer, MAX_MAZE_COLUMNS + 1);
		//	Assert if number of columns read doesn't match the number of columns
		//	in the first row
		int numColumns = file.gcount();
		//	If end-of-file not reached, the gcount() contains the newline
		//	character at the end of the current row; remove it from the current
		//	column count.
		if (!file.eof())
			numColumns -= 1;
		assert(numColumns == columns);
		rows++;
	}

    RoomSize size = this->_parseRoomSize(rows, columns);
    if (procedural) {
        if (size == RoomSize::INVALID) {
            std::cerr << "FATAL: invalid room size " << rows << "x" << columns << "\n";
            std::exit(1);
        }
    }
	//	Initialize Grid with the specified rows and columns
	Grid grid(rows, columns);

	//	Fill Grid with GridCells corresponding to characters in the maze file

	//	Reset file position
	file.seekg(file.beg);

	//	Populate Grid
	for (int row = 0; row < grid.getRows(); row++) {
		//	Read row from file
		file.getline(buffer, grid.getColumns() + 1);

		for (int col = 0; col < grid.getColumns(); col++) {
			char c = buffer[col];

			//	Identify CellType from character
			CellType type = charToCellType(c);

			//	Crash if CellType is unknown
			assert(type != CellType::Unknown);

			//	Create new GridCell
			grid.addCell(col, row, type);
		}
	}

	file.close();

    RoomClass rclass;
    rclass.entries = this->_identifyEntryways(grid);
    rclass.type = this->_getRoomType(path);
    rclass.size = size;


    if (procedural) {
        this->_validateRoom(grid, rclass);
    }

    int id = this->_next_room_id;
    switch (size) {
        case RoomSize::_10x10:
            this->_next_room_id++;
            break;
        case RoomSize::_20x20: // take up 4 id slots
            this->_next_room_id += 4;
            break;
        case RoomSize::_40x40: // take up 16 id slots
            this->_next_room_id += 16;
            break;
    };

    auto room = std::make_shared<Room>(std::move(grid), rclass, id);
    this->rooms_by_type.at(rclass.type).push_back(room);
    this->rooms_by_id.insert({id, room});
    this->rooms_by_class.insert({rclass, room});
}

RoomType MazeGenerator::_getRoomType(boost::filesystem::path path) {
    std::string extension = path.extension().string();

    if (extension == ".empty") {
        return RoomType::EMPTY;
    } else if (extension == ".spawn") {
        return RoomType::SPAWN;
    } else if (extension == ".easy") {
        return RoomType::EASY;
    } else if (extension == ".medium") {
        return RoomType::MEDIUM;
    } else if (extension == ".hard") {
        return RoomType::HARD;
    } else if (extension == ".loot") {
        return RoomType::LOOT;
    } else if (extension == ".exit") {
        return RoomType::EXIT;
    } else if (extension == ".maze") { // not procedural
        return RoomType::CUSTOM;
    }

    std::cerr << "FATAL: unknown file extension on room \""<< extension <<"\"\n";
    std::exit(1);
}

RoomSize MazeGenerator::_parseRoomSize(int rows, int columns) {
    if (rows == 10 && columns == 10) {
        return RoomSize::_10x10;
    } else if (rows == 20 && columns == 20) {
        return RoomSize::_20x20;
    } else if (rows == 40 && columns == 40) {
        return RoomSize::_40x40;
    } else {
        return RoomSize::INVALID;
    }
}

uint8_t MazeGenerator::_identifyEntryways(Grid& grid) {
    int row, column;
    bool has_entry;

    uint8_t entries;

    // Classify Top
    row = 0;
    has_entry = false;

    for (column = 0; column < grid.getColumns(); column++) {
        if (grid.getCell(column, row)->type == CellType::Empty) {
            has_entry = true;
            break;
        }
    }

    if (has_entry) {
        entries += RoomEntry::T;
    }

    // Classify Left
    column = 0;
    has_entry = false;

    for (row = 0; row < grid.getRows(); row++) {
        if (grid.getCell(column, row)->type == CellType::Empty) {
            has_entry = true;
            break;
        }
    }

    if (has_entry) {
        entries += RoomEntry::L;
    }

    // Classify Right
    column = grid.getColumns() - 1;
    has_entry = false;

    for (row = 0; row < grid.getRows(); row++) {
        if (grid.getCell(column, row)->type == CellType::Empty) {
            has_entry = true;
            break;
        }
    }

    if (has_entry) {
        entries += RoomEntry::R;
    }

    // Classify Bottom
    row = grid.getRows() - 1;
    has_entry = false;

    for (column = 0; column < grid.getColumns(); column++) {
        if (grid.getCell(column, row)->type == CellType::Empty) {
            has_entry = true;
            break;
        }
    }

    if (has_entry) {
        entries += RoomEntry::B;
    }

    return entries;
}

void MazeGenerator::_validateRoom(Grid& grid, const RoomClass& rclass) {
    auto* top_coords = &TOP_ENTRY_COORDS.at(rclass.size);
    auto* bottom_coords = &BOTTOM_ENTRY_COORDS.at(rclass.size);
    auto* left_coords = &LEFT_ENTRY_COORDS.at(rclass.size);
    auto* right_coords = &RIGHT_ENTRY_COORDS.at(rclass.size);

    const int NUM_SIDES = 4;

    std::array<uint8_t, NUM_SIDES> bits = {RoomEntry::T, RoomEntry::B, RoomEntry::L, RoomEntry::R};
    std::array<std::string, NUM_SIDES> labels = {"top", "bottom", "left", "right"};
    std::array<std::vector<glm::ivec2>*, NUM_SIDES> coords_arr = {top_coords, bottom_coords, left_coords, right_coords};

    for (int i = 0; i < NUM_SIDES; i++) {
        uint8_t bit = bits[i];
        const std::string& label = labels[i];
        const auto& curr_coords = coords_arr[i];

        int num_entries = 0;
        for (const auto& curr_coord : *curr_coords) {
            if (grid.getCell(curr_coord.x, curr_coord.y)->type == CellType::Empty) {
                num_entries++;
            }
        }

        bool should_have_entries = (rclass.entries & bit) != 0;
        int num_expected_entries;
        if (should_have_entries) {
            switch (rclass.size) {
                case RoomSize::_10x10:
                    num_expected_entries = 2;
                    break;
                case RoomSize::_20x20:
                    num_expected_entries = 4;
                    break;
                case RoomSize::_40x40:
                    num_expected_entries = 8;
                    break;
                default: 
                    std::cerr << "FATAL: WTF?? (@Tyler on Discord)\n";
                    break;
            }
        } else {
            num_expected_entries = 0;
        }

        if (num_entries != num_expected_entries) {
            std::cerr << "FATAL: expected " << num_expected_entries
                << " empty spaces on the " << label << " but found " << num_entries << "\n"
                << "If it looks correct to you, verify that you have them in the right coordinates.\n"
                << "If you do, I might have messed up in mazegenerator.hpp. @Tyler on Discord\n";
            std::exit(1);
        }
    }

    if (rclass.entries == 0) {
        std::cerr << "FATAL: room has no entrypoints? (Make sure they are in the correct positions)\n";
        std::exit(1);
    }
}

std::shared_ptr<Room> MazeGenerator::_pullRoomByPolicy() {
    if (_num_rooms_placed == REQUIRED_NUM_ROOMS - 1) {
        return _pullRoomByType(RoomType::EXIT);
    }

    RoomType type = _policy.front();
    _policy.pop_front();
    _policy.push_back(type);
    return _pullRoomByType(type);
}

std::shared_ptr<Room> MazeGenerator::_pullRoomByType(RoomType type) {
    int random_index = randomInt(0, this->rooms_by_type.at(type).size() - 1);

    return this->rooms_by_type.at(type).at(random_index);
}

std::vector<glm::ivec2> MazeGenerator::_getRoomCoordsTakenBy(RoomSize size, glm::ivec2 top_left) {
    std::vector<glm::ivec2> coords;

    switch (size) {
        case RoomSize::_40x40:
            coords.push_back(top_left + glm::ivec2(2, 0));
            coords.push_back(top_left + glm::ivec2(3, 0));
            coords.push_back(top_left + glm::ivec2(2, 1));
            coords.push_back(top_left + glm::ivec2(3, 1));
            coords.push_back(top_left + glm::ivec2(0, 2));
            coords.push_back(top_left + glm::ivec2(1, 2));
            coords.push_back(top_left + glm::ivec2(2, 2));
            coords.push_back(top_left + glm::ivec2(3, 2));
            coords.push_back(top_left + glm::ivec2(0, 3));
            coords.push_back(top_left + glm::ivec2(1, 3));
            coords.push_back(top_left + glm::ivec2(2, 3));
            coords.push_back(top_left + glm::ivec2(3, 3));
        case RoomSize::_20x20:
            coords.push_back(top_left + glm::ivec2(1, 0));
            coords.push_back(top_left + glm::ivec2(1, 1));
            coords.push_back(top_left + glm::ivec2(0, 1));
        case RoomSize::_10x10:
            coords.push_back(top_left);
            break;
        default:
            std::cerr << "FATAL: MazeGenerator::_getRoomCoordsTakenBy WTF?? @Tyler on Discord\n";
            std::exit(1);
    }

    return coords;
}

// bool MazeGenerator::_hasOpenConnection(std::shared_ptr<Room> room, glm::ivec2 origin_coord) {
//     for (const auto& coord : this->_getAdjRoomCoords(room, origin_coord)) {
//         if (this->maze[coord.y][coord.x] == UNUSED_TILE) {
//             return true;
//         }
//     }
//     return false;
// }

std::vector<std::pair<glm::ivec2, RoomEntry>> MazeGenerator::_getAdjRoomCoords(std::shared_ptr<Room> room, glm::ivec2 origin_coord) {
    std::vector<std::pair<glm::ivec2, RoomEntry>> adj_coords;
    auto coords = this->_getRoomCoordsTakenBy(room->rclass.size, origin_coord);

    if ((room->rclass.entries & RoomEntry::T) != 0) {
        adj_coords.push_back({origin_coord + glm::ivec2(0, -1), RoomEntry::B}); // need bottom entry for whatever would be placed here
        if (room->rclass.size == RoomSize::_20x20) {
            adj_coords.push_back({origin_coord + glm::ivec2(1, 1), RoomEntry::B});
        } else if (room->rclass.size == RoomSize::_40x40) {
            adj_coords.push_back({origin_coord + glm::ivec2(1, 1), RoomEntry::B});

            adj_coords.push_back({origin_coord + glm::ivec2(2, 1), RoomEntry::B});
            adj_coords.push_back({origin_coord + glm::ivec2(3, 1), RoomEntry::B});
        }
    }

    if ((room->rclass.entries & RoomEntry::L) != 0) {
        adj_coords.push_back({origin_coord + glm::ivec2(-1, 0), RoomEntry::R});
        if (room->rclass.size == RoomSize::_20x20) {
            adj_coords.push_back({origin_coord + glm::ivec2(-1, 1), RoomEntry::R});
        } else if (room->rclass.size == RoomSize::_40x40) {
            adj_coords.push_back({origin_coord + glm::ivec2(-1, 1), RoomEntry::R});

            adj_coords.push_back({origin_coord + glm::ivec2(-1, 2), RoomEntry::R});
            adj_coords.push_back({origin_coord + glm::ivec2(-1, 3), RoomEntry::R});
        }
    }

    if ((room->rclass.entries & RoomEntry::B) != 0) {
        if (room->rclass.size == RoomSize::_10x10) {
            adj_coords.push_back({origin_coord + glm::ivec2(0, 1), RoomEntry::T});
        } else if (room->rclass.size == RoomSize::_20x20) {
            adj_coords.push_back({origin_coord + glm::ivec2(0, 2), RoomEntry::T});
            adj_coords.push_back({origin_coord + glm::ivec2(1, 2), RoomEntry::T});
        } else if (room->rclass.size == RoomSize::_40x40) {
            adj_coords.push_back({origin_coord + glm::ivec2(0, 4), RoomEntry::T});
            adj_coords.push_back({origin_coord + glm::ivec2(1, 4), RoomEntry::T});
            adj_coords.push_back({origin_coord + glm::ivec2(2, 4), RoomEntry::T});
            adj_coords.push_back({origin_coord + glm::ivec2(3, 4), RoomEntry::T});
        }
    }

    if ((room->rclass.entries & RoomEntry::R) != 0) {
        if (room->rclass.size == RoomSize::_10x10) {
            adj_coords.push_back({origin_coord + glm::ivec2(1, 0), RoomEntry::L});
        } else if (room->rclass.size == RoomSize::_20x20) {
            adj_coords.push_back({origin_coord + glm::ivec2(2, 0), RoomEntry::L});
            adj_coords.push_back({origin_coord + glm::ivec2(2, 1), RoomEntry::L});
        } else if (room->rclass.size == RoomSize::_40x40) {
            adj_coords.push_back({origin_coord + glm::ivec2(4, 0), RoomEntry::L});
            adj_coords.push_back({origin_coord + glm::ivec2(4, 1), RoomEntry::L});
            adj_coords.push_back({origin_coord + glm::ivec2(4, 2), RoomEntry::L});
            adj_coords.push_back({origin_coord + glm::ivec2(4, 3), RoomEntry::L});
        }
    }

    return adj_coords;

}

void MazeGenerator::_placeRoom(std::shared_ptr<Room> room, glm::ivec2 origin_coord) {
    auto coords = _getRoomCoordsTakenBy(room->rclass.size, origin_coord);

    for (auto& coord : coords) {
        assert(!this->maze.contains(coord));

        this->maze.insert({coord, room->id});
    }


    for (const auto& [adj_coord, required_entry]: _getAdjRoomCoords(room, origin_coord)) {
        if (_isOpenWorldCoord(adj_coord)) {
            // ensure it has a bottom entry
            this->frontier.push({adj_coord, required_entry});
        }
    }
}

bool MazeGenerator::_isOpenWorldCoord(glm::ivec2 coord) {
    return (!this->maze.contains(coord));
}

std::vector<glm::ivec2> MazeGenerator::_getPossibleOriginCoords(std::shared_ptr<Room> room, RoomEntry required_entryway, glm::ivec2 coord) {
    std::vector<glm::ivec2> possible_origin_coords;

    if (room->rclass.size == RoomSize::_10x10) {
        possible_origin_coords.push_back(coord);
    } else if (room->rclass.size == RoomSize::_20x20) {
        switch (required_entryway) {
            case RoomEntry::T:
                possible_origin_coords.push_back(coord);
                possible_origin_coords.push_back(coord + glm::ivec2(-1, 0));
                break;
            case RoomEntry::B:
                possible_origin_coords.push_back(coord + glm::ivec2(0,  -1));
                possible_origin_coords.push_back(coord + glm::ivec2(-1, -1));
                break;
            case RoomEntry::L:
                possible_origin_coords.push_back(coord);
                possible_origin_coords.push_back(coord + glm::ivec2(0, -1));
                break;
            case RoomEntry::R:
                possible_origin_coords.push_back(coord + glm::ivec2(-1, 0));
                possible_origin_coords.push_back(coord + glm::ivec2(-1, -1));
                break;
        }
    } else if (room->rclass.size == RoomSize::_40x40) {
        switch (required_entryway) {
            case RoomEntry::T:
                possible_origin_coords.push_back(coord);
                possible_origin_coords.push_back(coord + glm::ivec2(-1, 0));
                possible_origin_coords.push_back(coord + glm::ivec2(-2, 0));
                possible_origin_coords.push_back(coord + glm::ivec2(-3, 0));
                break;
            case RoomEntry::B:
                possible_origin_coords.push_back(coord + glm::ivec2(0, -3));
                possible_origin_coords.push_back(coord + glm::ivec2(-1, -3));
                possible_origin_coords.push_back(coord + glm::ivec2(-2, -3));
                possible_origin_coords.push_back(coord + glm::ivec2(-3, -3));
                break;
            case RoomEntry::L:
                possible_origin_coords.push_back(coord);
                possible_origin_coords.push_back(coord + glm::ivec2(0, -1));
                possible_origin_coords.push_back(coord + glm::ivec2(0, -2));
                possible_origin_coords.push_back(coord + glm::ivec2(0, -3));
                break;
            case RoomEntry::R:
                possible_origin_coords.push_back(coord + glm::ivec2(-3, 0));
                possible_origin_coords.push_back(coord + glm::ivec2(-3, -1));
                possible_origin_coords.push_back(coord + glm::ivec2(-3, -2));
                possible_origin_coords.push_back(coord + glm::ivec2(-3, -3));
                break;
        }
    }

    return possible_origin_coords;
}

void MazeGenerator::_generatePolicy() {
    // the important thing here is the ratios

    using RatioMapping = const std::pair<int, RoomType>;


    RatioMapping NUM_EASY   = {7, RoomType::EASY}; // X easy
    RatioMapping NUM_MEDIUM = {5, RoomType::MEDIUM}; // for every Y mediums
    RatioMapping NUM_HARD   = {3, RoomType::HARD}; // for every Z hards
    RatioMapping NUM_LOOT   = {1, RoomType::LOOT}; // for every alpha loots

    for (const auto& [num, type] : {NUM_EASY, NUM_MEDIUM, NUM_HARD, NUM_LOOT}) {
        for (int i = 0; i < num; i++) {
            _policy.push_back(type);
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(_policy.begin(), _policy.end(), g);
}
