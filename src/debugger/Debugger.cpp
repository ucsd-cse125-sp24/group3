#include <iostream>
#include <string>
#include <unordered_map>
#include "shared/game/gamestate.hpp"
#include "debugger/debugger.hpp"

std::vector<std::string> get_string_tokens(std::string input, char delimeter) {
	//	Split string by delimiter
	std::vector<std::string> tokens;
	size_t delim_position;
	while ((delim_position = input.find(delimeter)) != std::string::npos) {
		//	Found delimeter position; add substring to tokens vector
		tokens.push_back(input.substr(0, delim_position));

		input = input.substr(delim_position + 1);
	}

	//	Push last token (after last delimeter)
	tokens.push_back(input);

	return tokens;
}

void initialize_commands(std::vector<Command*>& commands) {
	commands.push_back(new QuitCommand());
	commands.push_back(new StepCommand());
	commands.push_back(new StateCommand());
	commands.push_back(new PrintCommand());
	commands.push_back(new HelpCommand());
	commands.push_back(new CreateCommand());
	commands.push_back(new DeleteCommand());
	commands.push_back(new SetCommand());
}

void free_commands(std::vector<Command*> commands) {
	for (Command* c : commands) {
		delete c;
	}
}

void initialize_command_map(std::unordered_map<std::string, Command&>& command_map, std::vector<Command*> commands) {
	for (Command* c : commands) {
		command_map.insert({ c->name, *c });

		//	Add shorthand if the command has one
		if (c->shorthand.compare(NO_SHORTHAND) != 0)
		{
			//std::cout << c->name << " has a shorthand!\n";
			command_map.insert({ c->shorthand, *c });
		}
	}
}