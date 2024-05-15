#include "server/game/mazegenerator.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <memory>
#include <unordered_set>

#include <boost/graph/adjacency_matrix.hpp>
#include <boost/filesystem.hpp>

#include "shared/utilities/root_path.hpp"
#include "server/game/grid.hpp"
#include "server/game/gridcell.hpp"
#include "shared/utilities/rng.hpp"


MazeGenerator::MazeGenerator() {
    this->_next_room_id = 0;

    boost::filesystem::path rooms_dir = getRepoRoot() / "maps" / "rooms";

    boost::filesystem::path dir_10x10 = rooms_dir / "10x10";
    boost::filesystem::path dir_20x20 = rooms_dir / "20x20";
    boost::filesystem::path dir_40x40 = rooms_dir / "40x40";

    for (const auto & entry : boost::filesystem::directory_iterator(dir_10x10)) {
        this->_loadRoom(entry.path());
    }
    for (const auto & entry : boost::filesystem::directory_iterator(dir_20x20)) {
        this->_loadRoom(entry.path());
    }
    for (const auto & entry : boost::filesystem::directory_iterator(dir_40x40)) {
        this->_loadRoom(entry.path());
    }
}


Grid MazeGenerator::generate() {
    typedef boost::adjacency_matrix<boost::undirectedS> Graph;
    Graph graph(_next_room_id);

    auto spawn_room = _pullRoomByType(RoomType::SPAWN);

    glm::ivec2 spawn_room_coord(0, 0);
    for (const auto& coord : _getRoomCoordsTakenBy(spawn_room->rclass.size, spawn_room_coord)) {
        this->room_coords_taken.insert(coord);
    }

    std::shared_ptr<Room> curr_room = spawn_room;
    glm::ivec2 curr_origin_coord = spawn_room_coord;

    std::vector<std::pair<std::shared_ptr<Room>, glm::ivec2>> frontier;

    while (_hasOpenConnection(curr_room, curr_origin_coord)) {
        auto new_room = this->_pullRoomByType(RoomType::EMPTY);

        auto new_origin_coord = _tryToConnect(new_room, curr_room, curr_origin_coord);

        boost::add_edge(curr_room->id, new_room->id, graph);

        if (new_origin_coord.has_value()) {
            frontier.push_back({new_room, new_origin_coord.value()});
        }
    }

}


void MazeGenerator::_loadRoom(boost::filesystem::path path) {
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
    if (size == RoomSize::INVALID) {
        std::cerr << "FATAL: invalid room size " << rows << "x" << columns << "\n";
        std::exit(1);
    }
    std::cout << "\t" << rows << "x" << columns << '\n';

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
    this->_validateRoom(grid, rclass);

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
    this->rooms_by_type.insert({rclass.type, room});
    this->rooms_by_id.insert({id, room});
    this->rooms_by_class.insert({rclass, room});
    this->rooms_by_size.insert({size, room});

    std::cout << "\tLoaded\n";
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
    } else if (extension == ".diverse") {
        return RoomType::DIVERSE;
    } else if (extension == ".exit") {
        return RoomType::EXIT;
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

std::shared_ptr<Room> MazeGenerator::_pullRoomByType(RoomType type) {
    if (type != RoomType::EMPTY) {
        std::cerr << "only empty for testing\n";
        std::exit(1);
    }

    int random_index = randomInt(0, this->rooms_by_type.size() - 1);

    auto it = this->rooms_by_type.begin();
    std::advance(it, random_index);
    return it->second;
}

std::shared_ptr<Room> MazeGenerator::_pullRoomByClass(const RoomClass& rclass) {

}

std::vector<glm::ivec2> MazeGenerator::_getRoomCoordsTakenBy(RoomSize size, glm::ivec2 top_left) {
    std::vector<glm::ivec2> coords;

    switch (size) {
        case RoomSize::_40x40:
            coords.push_back(top_left + glm::ivec2(2, 0));
            coords.push_back(top_left + glm::ivec2(3, 0));
            coords.push_back(top_left + glm::ivec2(2, 1));
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

bool MazeGenerator::_hasOpenConnection(std::shared_ptr<Room> room, glm::ivec2 origin_coord) {
    for (const auto& coord : this->_getAdjRoomCoords(room, origin_coord)) {
        if (!this->room_coords_taken.contains(coord)) {
            return true;
        }
    }
    return false;
}

std::vector<glm::ivec2> MazeGenerator::_getAdjRoomCoords(std::shared_ptr<Room> room, glm::ivec2 origin_coord) {
    std::vector<glm::ivec2> adj_coords;
    auto coords = this->_getRoomCoordsTakenBy(room->rclass.size, origin_coord);

    if (room->rclass.entries & RoomEntry::T != 0) {
        adj_coords.push_back(origin_coord + glm::ivec2(0, 1));
        if (room->rclass.size == RoomSize::_20x20) {
            adj_coords.push_back(origin_coord + glm::ivec2(1, 1));
        } else if (room->rclass.size == RoomSize::_40x40) {
            adj_coords.push_back(origin_coord + glm::ivec2(1, 1));

            adj_coords.push_back(origin_coord + glm::ivec2(2, 1));
            adj_coords.push_back(origin_coord + glm::ivec2(3, 1));
        }
    }

    if (room->rclass.entries & RoomEntry::L != 0) {
        adj_coords.push_back(origin_coord + glm::ivec2(-1, 0));
        if (room->rclass.size == RoomSize::_20x20) {
            adj_coords.push_back(origin_coord + glm::ivec2(-1, 1));
        } else if (room->rclass.size == RoomSize::_40x40) {
            adj_coords.push_back(origin_coord + glm::ivec2(-1, 1));

            adj_coords.push_back(origin_coord + glm::ivec2(-1, 2));
            adj_coords.push_back(origin_coord + glm::ivec2(-1, 3));
        }
    }

    if (room->rclass.entries & RoomEntry::B != 0) {
        if (room->rclass.size == RoomSize::_10x10) {
            adj_coords.push_back(origin_coord + glm::ivec2(0, 1));
        } else if (room->rclass.size == RoomSize::_20x20) {
            adj_coords.push_back(origin_coord + glm::ivec2(0, 2));
            adj_coords.push_back(origin_coord + glm::ivec2(1, 2));
        } else if (room->rclass.size == RoomSize::_40x40) {
            adj_coords.push_back(origin_coord + glm::ivec2(0, 4));
            adj_coords.push_back(origin_coord + glm::ivec2(1, 4));
            adj_coords.push_back(origin_coord + glm::ivec2(2, 4));
            adj_coords.push_back(origin_coord + glm::ivec2(3, 4));
        }
    }

    if (room->rclass.entries & RoomEntry::R != 0) {
        if (room->rclass.size == RoomSize::_10x10) {
            adj_coords.push_back(origin_coord + glm::ivec2(1, 0));
        } else if (room->rclass.size == RoomSize::_20x20) {
            adj_coords.push_back(origin_coord + glm::ivec2(2, 0));
            adj_coords.push_back(origin_coord + glm::ivec2(2, 1));
        } else if (room->rclass.size == RoomSize::_40x40) {
            adj_coords.push_back(origin_coord + glm::ivec2(4, 0));
            adj_coords.push_back(origin_coord + glm::ivec2(4, 1));
            adj_coords.push_back(origin_coord + glm::ivec2(4, 2));
            adj_coords.push_back(origin_coord + glm::ivec2(4, 3));
        }
    }

    return adj_coords;

}

std::optional<glm::ivec2> _tryToConnect(std::shared_ptr<Room> new_room, std::shared_ptr<Room> old_room, glm::ivec2 old_origin) {

    // todo: tries to connect old_room to new_room and if possible returns new origin coord
}