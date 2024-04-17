#include <iostream>
#include <string>
#include <unordered_map>
#include "shared/game/gamestate.hpp"
#include "shared/game/gamelogic/constants.hpp"
#include "debugger/debugger.hpp"

std::vector<Command*> commands;

std::unordered_map<std::string, Command&> command_map;

/**
 * @brief This is a mock of how the Server could work, simulating updating the
 * abstract game state after every time step.
 *
 * This prototype acts as a simple shell that will allow the user to place input
 * commands to create / update / remove objects in the GameState instance as
 * well as to progress to the next time step. The goal here is to simulate
 * client events.
 *
 */
int main(int argc, char* argv[]) {
	//	GameState "Debugger"

	//	1.	Create a GameState object
	GameState state(GamePhase::GAME);

	//	2.	Fill it with some objects
	Object* obj1 = state.createObject();
	obj1->setPosition(glm::vec3(0.f, 0.f, 0.f));
	obj1->setVelocity(glm::vec3(0.f, 0.f, 10.f));
	obj1->setAcceleration(glm::vec3(0.f, 0.f, -1.f));

	Object* obj2 = state.createObject();
	obj2->setPosition(glm::vec3(0.f, 0.f, 0.f));
	obj2->setVelocity(glm::vec3(1.f, 1.f, 10.f));
	obj2->setAcceleration(glm::vec3(1.f, -1.f, 0.f));

	//	3.	Start debugger shell

	//	Initial printouts
	std::cout << "GameState Debugger" << std::endl;
	std::cout << "Current timestep: " << state.getTimestep();
	std::cout << " | Timestep Length: " << state.getTimestepLength().count() << std::endl;

	//	4.	Load all debugger commands
	std::cout << "Loading debugger commands...";

	initialize_commands(commands);

	initialize_command_map(command_map, commands);

	std::cout << "done." << std::endl;

	//	User input string
	std::string input;
	std::vector<std::string> tokens;

	while (true) {
		//	Print prompt
		std::cout << "(gsdb) ";

		//	Get user input
		std::getline(std::cin, input);

		//	Parse user input
		tokens = get_string_tokens(input, ' ');

		//	Try to find command in command_map
		std::unordered_map<std::string, Command&>::iterator c_iter = command_map.find(tokens.at(0));

		if (c_iter == command_map.end())
		{
			std::cout << "Unknown command '" << tokens.at(0) << "'.\n";
		}
		else {
			Command& c = c_iter->second;

			c.run(tokens, state);
		}
	}
}