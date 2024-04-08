## Game Name Here

TODO: Game description here, with screenshots.

## Setup

### Dependencies

1. CMake
    - Minimum version 3.13
    - [Install Instructions](https://cgold.readthedocs.io/en/latest/first-step/installation.html)
2. g++ / Mingw64
    - Should be new enough support C++20 features
    - [Install Instructions]()
3. OpenGL
    - TODO: decide on version
    - TODO: link install instructions here

### Steps 

1. Install Dependencies
2. Create a directory called `build`
3. From inside the `build` directory, run `cmake` like so:
    ```sh
    cmake ..
    ```
4. Then, you can build the `server` and `client` executables with these commands:
    ```sh
    make server # just make the server
    make client # just make the client
    make        # make server & client
    ```
5. Then, to run the executables:
    ```sh
    bin/server # run the server
    bin/client # run the client
    ```

Note that the full filepaths for the executables are `build/bin/server` and `build/bin/client`, so if you are outside of the build directory you will need to give the full path.

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
