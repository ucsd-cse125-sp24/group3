#pragma once
#include "server/game/servergamestate.hpp"
#include "server/game/constants.hpp"

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
	 * @param state Reference to the ServerGameState instance maintained by the
	 * debugger.
	 */
	virtual void run(std::vector<std::string> arguments, ServerGameState& state) = 0;
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

	void run(std::vector<std::string> arguments, ServerGameState& state) override { //cppcheck-suppress passedByValue
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

	void run(std::vector<std::string> arguments, ServerGameState& state) override { // cppcheck-suppress passedByValue
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

	void run(std::vector<std::string> arguments, ServerGameState& state) override { // cppcheck-suppress passedByValue
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

	void run(std::vector<std::string> arguments, ServerGameState& state) override { //cppcheck-suppress passedByValue
		//	Variations:
		//	print [global id number] - print state of object with given id
		//	print [global id number] [property] - print state of given property

		if (arguments.size() == 1) {
			std::cout << "Error: Incorrect number of arguments for 'print' command\n";
		}
		else if (arguments.size() >= 2 && arguments.size() < 4) {
			//	print [global id number]
			//	Verify that second argument is an integer
			unsigned int id;
			try {
				id = std::stoi(arguments.at(1));
			}
			catch (...) {
				std::cout << "Error: invalid argument for 'print' command - expected an integer.\n";
				return;
			}

			Object* object = state.objects.getObject(id);

			if (object == nullptr) {
				std::cout << "No object with id " << arguments.at(1) << " exists.\n";
				return;
			}

			if (arguments.size() == 2) {
				//	Print entire object state
				std::cout << object->to_string() << std::endl;
			}
			else if (arguments.size() == 3) {
				//	print [global id number] [property]

				//	Attempt to print object's given property (must be a better
				//	way to do this)
				std::string property = arguments.at(2);

				if (property.compare("globalID") == 0) {
					std::cout << object->globalID << std::endl;
				}
				else if (property.compare("typeID") == 0) {
					std::cout << object->typeID << std::endl;
				}
				else if (property.compare("type") == 0) {
					std::cout << objectTypeString(object->type) << std::endl;
				}
				else if (property.compare("physics") == 0) {
					std::cout << object->physics.to_string() << std::endl;
				}
				else if (property.compare("physics.movable") == 0) {
					std::cout << (object->physics.movable ? "true" : "false") << std::endl;
				}
				else if (property.compare("physics.shared.position") == 0) {
					std::cout << glm::to_string(object->physics.shared.position) << std::endl;
				}
				else if (property.compare("physics.velocity") == 0) {
					std::cout << glm::to_string(object->physics.velocity) << std::endl;
				}
				else if (property.compare("physics.acceleration") == 0) {
					std::cout << glm::to_string(object->physics.acceleration) << std::endl;
				}
				else if (property.compare("physics.shared.facing") == 0) {
					std::cout << glm::to_string(object->physics.shared.facing) << std::endl;
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

	void run(std::vector<std::string> arguments, ServerGameState& state) override { //cppcheck-suppress passedByValue
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

	void run(std::vector<std::string> arguments, ServerGameState& state) override { // cppcheck-suppress passedByValue
		//	This command ignores arguments

		//	Create a new base object in the game state
		unsigned int globalID = state.objects.createObject(ObjectType::Object);
		Object* obj = state.objects.getObject(globalID);

		std::cout << "Created new object (global id " << globalID << ")" << std::endl;
	}
};

class DeleteCommand : public Command {
public:
	DeleteCommand() {
		this->name = "delete";
		this->shorthand = "d";
	}

	void run(std::vector<std::string> arguments, ServerGameState& state) override { // cppcheck-suppress passedByValue
		//	delete [object global id]
		if (arguments.size() != 2) {
			std::cout << "Error: Incorrect number of arguments for 'delete' command.\n";
			return;
		}

		//	Get global id argument
		EntityID id;
		try {
			id = (EntityID) std::stoi(arguments.at(1));
		}
		catch (...) {
			std::cout << "Error: invalid argument for 'delete' command - expected an integer.\n";
			return;
		}

		//	Attempt to remove object with the given id
		bool success = state.objects.removeObject(id);

		if (success) {
			std::cout << "Deleted object (global id " << id << ")" << std::endl;
		}
		else {
			std::cout << "Failed to delete object (global id " << id << ") - object does not exist.\n";
		}
	}
};

class SetCommand : public Command {
public:
	SetCommand() {
		this->name = "set";
		this->shorthand = NO_SHORTHAND;
	}

	void run(std::vector<std::string> arguments, ServerGameState& state) override { //cppcheck-suppress passedByValue
		//	set [object global id] [property] [new value]
		if (arguments.size() != 4) {
			std::cout << "Error: Incorrect number of arguments for 'set' command.\n";
			return;
		}

		//	Get global id argument
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
		Object* obj = state.objects.getObject(id);

		if (obj == nullptr) {
			std::cout << "No object with global id " << id << " exists.\n";
			return;
		}

		//	Set property
		if (property.compare("physics.shared.position.x") == 0) {
			obj->physics.shared.position.x = value;
			std::cout << "Set object (global id " << id << ") position.x to " << value << ".\n";
		}
		else if (property.compare("physics.shared.position.y") == 0) {
			obj->physics.shared.position.y = value;
			std::cout << "Set object (global id " << id << ") position.y to " << value << ".\n";
		}
		else if (property.compare("physics.shared.position.z") == 0) {
			obj->physics.shared.position.z = value;
			std::cout << "Set object (global id " << id << ") position.z to " << value << ".\n";
		}
		else if (property.compare("physics.velocity.x") == 0) {
			obj->physics.velocity.x = value;
			std::cout << "Set object (global id " << id << ") velocity.x to " << value << ".\n";
		}
		else if (property.compare("physics.velocity.y") == 0) {
			obj->physics.velocity.y = value;
			std::cout << "Set object (global id " << id << ") velocity.y to " << value << ".\n";
		}
		else if (property.compare("physics.velocity.z") == 0) {
			obj->physics.velocity.z = value;
			std::cout << "Set object (global id " << id << ") velocity.z to " << value << ".\n";
		}
		else if (property.compare("physics.acceleration.x") == 0) {
			obj->physics.acceleration.x = value;
			std::cout << "Set object (global id " << id << ") acceleration.x to " << value << ".\n";
		}
		else if (property.compare("physics.acceleration.y") == 0) {
			obj->physics.acceleration.y = value;
			std::cout << "Set object (global id " << id << ") acceleration.y to " << value << ".\n";
		}
		else if (property.compare("physics.acceleration.z") == 0) {
			obj->physics.acceleration.z = value;
			std::cout << "Set object (global id " << id << ") acceleration.z to " << value << ".\n";
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