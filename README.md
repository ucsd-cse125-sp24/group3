## Game Name Here

TODO: Game description here, with screenshots.

## Setup

### Windows (In Visual Studio)

1. Download Visual Studio (if you are on a lab computer, it will already be installed)
2. Clone the repo
    - In Visual Studio you should have gui option to clone from a Github Repo
		- in the popup window put in this URL: `https://github.com/ucsd-cse125-sp24/group3.git`
    - Alternatively, you can use the command line and then open Visual Studio in the directory where you cloned it:
		- `git clone https://github.com/ucsd-cse125-sp24/group3.git`
3. Everything should just work™. To run the program, you can click on the green arrow at the top bar that says "Select Startup Item." There is a dropdown arrow on the right side which will let you select between running the client and the server.

### Linux / Mac

1. Download these dependencies:
	1. `cmake`
		- Minimum version 3.13
		- [Install Instructions](https://cgold.readthedocs.io/en/latest/first-step/installation.html)
	2. `g++`
		- Should be new enough support C++20 features
		- Mac: [here](https://formulae.brew.sh/formula/gcc)
		- Linux: use your favorite package manager, if it isn't already installed.
	3. `make`
		- Mac: [here](https://formulae.brew.sh/formula/make).
		- Linux: it probably is already installed on your system, but if not use your favorite package manager.
	4. OpenGL
		- TODO: figure out what version the lab computers have installed, and then put instructions to install it here.
2. Verify installations:
    - Enter in `cmake`, `g++`, and `make` into your terminal
    - Confirm that each of these commands are recognized by your system
    - If not, you may have to modify your system path. But generally your system package manager (e.g. `homebrew`, `apt`) should do this for you, so you should verify that your package manager actually installed the program.
3. Clone the repo
    - Go to your favorite location in your file system
    - `git clone https://github.com/ucsd-cse125-sp24/group3.git`.
    - `cd group3`
4. Once you are in the directory you cloned, enter the following commands:
    - `mkdir build`
    - `cd build`
    - `cmake ..`
    - Let CMake run...
5. CMake should have generated a bunch of stuff inside of the build directory. One of these things should be a Makefile. Verify that there is a Makefile in the `build` directory.
6. To compile the program, enter these commands (depending on what you want to do)
    - `make` (to compile everything)
    - `make client` (to compile the client executable)
    - `make server` (to compile the server executable)
7. To run one of the executables, enter the following (from the build directory)
    - `bin/client` (to run the client)
    - `bin/server` (to run the server)
	- Note that the full filepaths for the executables are `build/bin/server` and `build/bin/client`, so if you are outside of the build directory you will need to give the full path.

### Windows (Without Visual Studio)

I'm keeping this more for legacy reasons since this doesn't work on the lab computers, but if you want to use windows but don't want to use the Visual Studio IDE, then this is one way you can get it to work.

This also is relatively janky. On my Desktop computer, for some reason if you try and compile a C++ program using g++ from Powershell it works fine, but if you try and do it from Git Bash it seg faults (even in a simple Hello World program).

1. Download Dependencies
	1. `cmake`
		- Minimum version 3.13
		- [Install Instructions](https://cgold.readthedocs.io/en/latest/first-step/installation.html)
	2. `g++`
		- Follow [this](https://www.freecodecamp.org/news/how-to-install-c-and-cpp-compiler-on-windows/) tutorial.
	3. `make`
		- Inside of the `msys2 MINGW64` terminal (the above tutorial should have had you install it), enter `pacman -S make`.
		- Then, go back to your environment variable settings and add `C:\msys64\usr\bin` to your system path.
			- (This is at `View Advanced System Settings` > `Environment Variables` > Double Click on System Path at the bottom)
		- Make sure that in powershell if you type in `make` it recognizes the command. That is how you know it is installed correctly.
	4. OpenGL
		- TODO: decide on version
		- TODO: link install instructions here
2. Follow the same steps as the Linux / Mac instructions. You may need to add some extra information when you run `CMake`.
    - Instead of running `cmake ..`, you might have to run `cmake -G "Unix Makefiles" ..`

I would generally recommend using Visual Studio if you are working on Windows, since it seems to just work, but I was also able to get this working.

## Development

### Code Structure

The repo is structured into 3 main sections:

- `client`: code for the client
- `server`: code for the server
- `shared`: shared library code that both the client and server depend upon

In addition, there is a distinction between the `include` and `src` directories. Internally, these directories have very similar structure, but the `include` directory includes all of the header (.hpp) files and the src directory includes all of the source files (.cpp).

### CMake Structure

There are 4 `CMakeLists.txt` files throughout the repo.

- `CMakeLists.txt`: high level set up of CMake itself
- `src/shared/CMakeLists.txt`: handles the creation of the shared library which is later linked into the client and server executables
- `src/client/CMakeLists.txt`: handles the creation of the client executable, which includes linking in our own shared library code and any other 3rd party libraries needed for the client.
- `src/server/CMakeLists.txt`: same as above, but for the server.

### `#include`ing Header Files

To include a header file, make sure you use the full file path starting from the `include` directory. So, for example, to include the `GameState` class in some source file, you would put at the top

```cpp
#include "shared/game/gamestate.hpp"
```

### Adding New Files

To add a new file it is fairly simple, but you just have to make sure that you update the corresponding `CMakeLists.txt` file so that CMake includes your src file in the compilation. (Header files will automatically be included once they are placed).

For example, if you want to add a file somewhere in the shared library section of the repo, make sure you add the filename to `src/shared/CMakeLists.txt`.

### Adding New Libraries

Depending on where you need to link the library (client, server, shared), you will need to go to that `CMakeLists.txt` file and include the relevant CMake incantations/spells/general wizardry to link that library into the specific target. This will likely vary depending on what you are trying to add.

## Useful VSCode Extensions

- [C++ Intellisense](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- [General Productivity](https://marketplace.visualstudio.com/items?itemName=jirkavrba.subway-surfers)
