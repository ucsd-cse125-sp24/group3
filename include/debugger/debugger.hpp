#pragma once
#include "shared/game/gamestate.hpp"
#include "shared/game/gamelogic/constants.hpp"

const std::string NO_SHORTHAND = "NO_SHORTHAND";

/**
 * @brief This is the Command class from which all debugger commands derive.
 * Each debugger command has a name and a shorthand name. The
 * run() method is overloaded by each particular command to implement its
 * particular behavior.
 */
class Command {
public:
	/**
	 * @brief The Command's name. When the user enters an input string, the
	 * first word is treated as the intended command name, and if the debugger
	 * finds a command with a matching name, it will attempt to run it.
	 */
	std::string name;

	/**
	 * @brief Alternative shorthand name that also refers to this command.
	 */
	std::string shorthand;

	/**
	 * @brief The Command's run() method is overloaded by each particular
	 * command to implement its particular behavior.
	 * @param arguments Vector of string arguments from user
	 * @param state Reference to the GameState instance maintained by the
	 * debugger.
	 */
	virtual void run(std::vector<std::string> arguments, GameState& state) = 0;
};

//	Vector of all commands known by the debugger
extern std::vector<Command*> commands;

//	Hashmap of string names / shorthands -> Command references (used for command
//	lookup)
extern std::unordered_map<std::string, Command&> command_map;

/**
 * @brief Allocate all Command objects recognized by the debugger.
 * @param commands
 */
void initialize_commands(std::vector<Command*>& commands);

/** 
 * @brief  Deallocate all Command objects allocated by initialize_commands().
 * @param commands  
 */
void free_commands(const std::vector<Command*>& commands);

//	Debugger Commands

class QuitCommand : public Command {
public:
	QuitCommand() {
		this->name = "quit";
		this->shorthand = "q";
	}

	void run(std::vector<std::string> arguments, GameState& state) override {
		//	This command ignores arguments
		std::cout << "Quitting gsdb..." << std::endl;

		free_commands(commands);
		exit(0);
	}
};

class StepCommand : public Command {
public:
	StepCommand() {
		this->name = "step";
		this->shorthand = "s";
		//this->num_expected_args = 0;
	}

	void run(std::vector<std::string> arguments, GameState& state) override {
		//	Possible variations:
		//	step - call GameState::update() once on the given state instance.
		//	step n - call update() n times.

		if (arguments.size() == 1) {
			//	step variation
			//	Update to the next timestep
			state.update();
			std::cout << "Current timestep: " << state.getTimestep() << std::endl;
		}
		else if (arguments.size() == 2) {
			//	step n variation

			//	Verify that second argument is an integer
			unsigned int n;
			try {
				n = std::stoi(arguments.at(1));
			}
			catch (...) {
				std::cout << "Error: invalid argument for 'step' command - expected an integer.\n";
				return;
			}

			//	Call update() n times
			for (unsigned int i = 0; i < n; i++) {
				state.update();
			}

			std::cout << "Called update() " << arguments.at(1) << " times." << std::endl;
			std::cout << "Current timestep: " << state.getTimestep() << std::endl;
		}
		else {
			std::cout << "Error: Incorrect number of arguments for 'step' command.\n";
		}
	}
};

class StateCommand : public Command {
public:
	StateCommand() {
		this->name = "state";
		this->shorthand = NO_SHORTHAND;
	}

	void run(std::vector<std::string> arguments, GameState& state) override {
		//	This command ignores arguments

		//	Print GameState instance' state
		std::cout << state.to_string() << std::endl;
	}
};

class PrintCommand : public Command {
public:
	PrintCommand() {
		this->name = "print";
		this->shorthand = "p";
	}

	void run(std::vector<std::string> arguments, GameState& state) override {
		//	Variations:
		//	print [id number] - print state of object with given id
		//	print [id number] [property] - print state of given property

		if (arguments.size() == 1) {
			std::cout << "Error: Incorrect number of arguments for 'print' command\n";
		}
		else if (arguments.size() >= 2 && arguments.size() < 4) {
			//	print [id number]
			//	Verify that second argument is an integer
			unsigned int id;
			try {
				id = std::stoi(arguments.at(1));
			}
			catch (...) {
				std::cout << "Error: invalid argument for 'print' command - expected an integer.\n";
				return;
			}

			Object* object = state.getObject(id);

			if (object == nullptr) {
				std::cout << "No object with id " << arguments.at(1) << " exists.\n";
				return;
			}

			if (arguments.size() == 2) {
				//	Print entire object state
				std::cout << object->to_string() << std::endl;
			}
			else if (arguments.size() == 3) {
				//	print [id number] [property]

				//	Attempt to print object's given property (must be a better
				//	way to do this)
				std::string property = arguments.at(2);

				if (property.compare("id") == 0) {
					std::cout << object->id << std::endl;
				}
				else if (property.compare("position") == 0) {
					std::cout << glm::to_string(object->position) << std::endl;
				}
				else if (property.compare("velocity") == 0) {
					std::cout << glm::to_string(object->velocity) << std::endl;
				}
				else if (property.compare("acceleration") == 0) {
					std::cout << glm::to_string(object->acceleration) << std::endl;
					
				}
				else {
					std::cout << "Error: Didn't recognize object property '" << property << "'.\n";
				}
			}
		}
		else {
			std::cout << "Error: Incorrect number of arguments for 'print' command.\n";
		}
	}
};

class HelpCommand : public Command {
public:
	HelpCommand() {
		this->name = "help";
		this->shorthand = NO_SHORTHAND;
	}

	void run(std::vector<std::string> arguments, GameState& state) override {
		//	This command ignores arguments (though it may make sense to have
		//	command descriptors, e.g. help step -> "the step command does ..."

		std::cout << "The GameState Debugger understands the following commands:\n";

		for (Command* command : commands) {
			std::cout << command->name;

			if (command->shorthand.compare(NO_SHORTHAND) != 0)
				std::cout << " (" << command->shorthand << ")";

			std::cout << std::endl;
		}
	}
};

class CreateCommand : public Command {
public:
	CreateCommand() {
		this->name = "create";
		this->shorthand = "c";
	}

	void run(std::vector<std::string> arguments, GameState& state) override {
		//	This command ignores arguments

		//	Create a new object in the game state
		const Object* obj = state.createObject();

		std::cout << "Created new object (id " << obj->id << ")" << std::endl;
	}
};

class DeleteCommand : public Command {
public:
	DeleteCommand() {
		this->name = "delete";
		this->shorthand = "d";
	}

	void run(std::vector<std::string> arguments, GameState& state) override {
		//	delete [object id]
		if (arguments.size() != 2) {
			std::cout << "Error: Incorrect number of arguments for 'delete' command.\n";
			return;
		}

		//	Get id argument
		unsigned int id;
		try {
			id = std::stoi(arguments.at(1));
		}
		catch (...) {
			std::cout << "Error: invalid argument for 'delete' command - expected an integer.\n";
			return;
		}

		//	Attempt to remove object with the given id
		bool success = state.removeObject(id);

		if (success) {
			std::cout << "Deleted object (id " << id << ")" << std::endl;
		}
		else {
			std::cout << "Failed to delete object (id " << id << ") - object does not exist.\n";
		}
	}
};

class SetCommand : public Command {
public:
	SetCommand() {
		this->name = "set";
		this->shorthand = NO_SHORTHAND;
	}

	void run(std::vector<std::string> arguments, GameState& state) override {
		//	set [object id] [property] [new value]
		if (arguments.size() != 4) {
			std::cout << "Error: Incorrect number of arguments for 'set' command.\n";
			return;
		}

		//	Get id argument
		unsigned int id;
		try {
			id = std::stoi(arguments.at(1));
		}
		catch (...) {
			std::cout << "Error: invalid argument for 'set' command - expected an integer.\n";
			return;
		}

		//	Get property
		std::string property = arguments.at(2);

		//	Get new value
		int value;
		try {
			value = std::stoi(arguments.at(3));
		}
		catch (...) {
			std::cout << "Error: invalid argument for 'set' command - expected an integer.\n";
			return;
		}

		//	Try to get object
		Object* obj = state.getObject(id);

		if (obj == nullptr) {
			std::cout << "No object with id " << id << " exists.\n";
			return;
		}

		//	Set property
		if (property.compare("id") == 0) {
			obj->id = value;
			std::cout << "Set object (original id " << id << ") id to " << value << ".\n";
		}
		else if (property.compare("position.x") == 0) {
			obj->position.x = value;
			std::cout << "Set object (id " << id << ") position.x to " << value << ".\n";
		}
		else if (property.compare("position.y") == 0) {
			obj->position.y = value;
			std::cout << "Set object (id " << id << ") position.y to " << value << ".\n";
		}
		else if (property.compare("position.z") == 0) {
			obj->position.z = value;
			std::cout << "Set object (id " << id << ") position.z to " << value << ".\n";
		}
		else if (property.compare("velocity.x") == 0) {
			obj->velocity.x = value;
			std::cout << "Set object (id " << id << ") velocity.x to " << value << ".\n";
		}
		else if (property.compare("velocity.y") == 0) {
			obj->velocity.y = value;
			std::cout << "Set object (id " << id << ") velocity.y to " << value << ".\n";
		}
		else if (property.compare("velocity.z") == 0) {
			obj->velocity.z = value;
			std::cout << "Set object (id " << id << ") velocity.z to " << value << ".\n";
		}
		else if (property.compare("acceleration.x") == 0) {
			obj->acceleration.x = value;
			std::cout << "Set object (id " << id << ") acceleration.x to " << value << ".\n";
		}
		else if (property.compare("acceleration.y") == 0) {
			obj->acceleration.y = value;
			std::cout << "Set object (id " << id << ") acceleration.y to " << value << ".\n";
		}
		else if (property.compare("acceleration.z") == 0) {
			obj->acceleration.z = value;
			std::cout << "Set object (id " << id << ") acceleration.z to " << value << ".\n";
		}
		else {
			std::cout << "Error: Didn't recognize object property '" << property << "'.\n";
		}
	}
};

//	Utility Functions for Debugger Parsing

/**
 * @brief  Reads an input string and splits it into a vector of strings by a
 * character delimeter.
 * @param input  String to split into tokens
 * @param delimeter  Delimiter by which to split string into tokens (no token
 * will contain the delimeter character)
 * @return  vector<string> of string tokens
 */
std::vector<std::string> get_string_tokens(std::string input, char delimeter);

/**
 * @brief  Initialize string (name) -> Command * hashmap maintained by the
 * debugger to all known commands.
 * @param command_map  Reference to the debugger's command map (assumed empty).
 */
void initialize_command_map(std::unordered_map<std::string, Command&>& command_map, const std::vector<Command*>& commands);