# Game of Life
_Gilbert Francois Duivesteijn_

This program shows the well known Game of Life. I've chosen this simulation as an exercise to explore:

* Member function pointers as function arguments,
* Concurrency, domain slicing and treating boundary conditions,
* Development of a multi platform, multi architecture console and graphical application.



|                                            |                                                  |
| ------------------------------------------ | ------------------------------------------------ |
| ![CLI](./assets/images/screenshot_cli.png) | ![CLI](./assets/images/screenshot_gui_zoom8.png) |
| Terminal version                           | GUI version (zoom = 8)                           |



## Building on macOS or Linux

The project uses [vcpkg dependency manager](https://vcpkg.io) and is included as a sub-repository in this project. It will automatically download and build libSDL2 for you. 

When cloning the project, don't forget the `--recurse-submodules` option.

```sh
# Clone the project
git clone --recurse-submodules https://github.com/gilbertfrancois/game-of-life.git

cd game-of-life

# Bootstrap vcpkg
./3rdparty/vcpkg/bootstrap-vcpkg.sh

# Build the project and its dependencies
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make

```



## Building on Windows with Visual Studio 2022

To do...



## Building on Windows with Visual Studio (2019 and before)

- Clone the project from https://github.com/gilbertfrancois/game-of-life.git
- Download `SDL2-devel-2.xx-VC.zip` from [https://github.com/libsdl-org/SDL/releases](https://github.com/libsdl-org/SDL/releases), where 2.xx is the latest version that is available.  Unzip and copy the files e.g. in: `game-of-life/3rdparty/sdl2` so that the project folder structure looks like:

	```sh
	C:...path to...\game-of-life>
	├───3rdparty
	│   └───sdl2
	│       ├───cmake
	│       ├───docs
	│       ├───include
	│       └───lib
	│           ├───x64
	│           └───x86
	├───cmake
	└───src
    ├───cli
    ├───gui
    └───lib
	```

- In Visual Studio, go to Project -> CMake settings and add the variable SDL2_DIR:

|                                                              |
| ------------------------------------------------------------ |
| ![Visual Studio settings](./assets/images/vs2022_cmake_settings.png) |
| Projects -> CMake settings \| CMake variables and cache -> **SDL2_DIR <path>** |

- Build the project

- Copy `3rdparty/sdl2/lib/SDL2.dll` to the build folder, where game-of-life-gui.exe is located.



## Running the programs

Open a terminal and type:

```sh
./game-of-life-cli
```

for the text terminal version and

```sh
./game-of-life-gui --fullscreen
```

for the GUI version in full screen. The CLI and GUI programs have different options:

```sh
game-of-life-gui [options]

   --fullscreen          : display full screen.
   --zoom <number>       : zoom factor, default = 1.
   --steps <number>      : number of steps, default = 1000.
   --bt <number>         : boundary type: 0=const, 1=periodic, 2=mirror, default=1.
   --without-threads     : compute single threaded.
   --with-threads        : compute multi-threaded.
   -h, --help            : info and help message.
```

```sh
game-of-life-cli [options]

   --width <number>      : width of the domain, default is current terminal width.
   --height <number>     : height of the domain, default is current terminal height.
   --steps <number>      : number of steps, default = 1000.
   --bt <number>         : boundary type: 0=const, 1=periodic, 2=mirror, default=1.
   --without-threads     : compute single threaded.
   --with-threads        : compute multi-threaded.
   -h, --help            : info and help message.
```

The GUI can be terminated with `[q]` or `[esc]`.



## Boudary conditions

There are 3 possible boundary conditions:

| Number | Type     | Description                                                  |
| ------ | -------- | ------------------------------------------------------------ |
| 0      | Constant | All cells on the perimeter are dead.                         |
| 1      | Periodic | The neighbors to a cell at the edge of the grid are those cells at the opposite edge of the grid. |
| 2      | Mirror   | The neightbors to a cell at the edge have the same value as the cell in the normal direction of the edge. |



## (Member) function pointers as function arguments

If you want to use function pointers as argument, where the function is a member function of a class, you also have to parse a pointer of the instance of the object. At the end of this paragraph, there is a small code example showing the different cases for C style and C++ style function pointers.

The Game of Life simulation uses one function method for domain slicing and distributing the work load over the available CPU cores by starting an equal amount of threads. It takes a function pointer as an argument, to enable me to send the function for initial conditions and the function for time stepping to this method.



**main.cpp**

```c++
#include <cmath>
#include "Foo.h"

int main() {
    Foo *foo = new Foo;

    foo->printOutputCFunction(sin, 1.57);
    foo->printOutputCppMemberFunction(&Foo::add, foo, 1, 2);
    foo->printOutputCppMemberFunctionSelf();
    return 0;
}
```

**Foo.h**
```c++
//
// Created by Gilbert François on 07-09-16.
//
#ifndef FUNCTIONPOINTERS_FOO_H
#define FUNCTIONPOINTERS_FOO_H

class Foo {

public:

    int add(int i, int j);

    // C style for calling function pointer
    void printOutputCFunction(double (*fn)(double), double x);

    // C++ style for calling member functions. It needs a second argument for the reference to the instance
    void printOutputCppMemberFunction(int (Foo::*fn)(int, int), Foo* foo, int i, int j);

    // C++ style for calling member functions, where the object instance calls one of its own functions and providing
    // the instance pointer by the reference 'this'.
    void printOutputCppMemberFunctionSelf();
};


#endif //FUNCTIONPOINTERS_FOO_H
```

**Foo.cpp**
```c++
//
// Created by Gilbert François on 07-09-16.
//
#include <iostream>
#include "Foo.h"

int Foo::add(int i, int j) {
    return i + j;
}

void Foo::printOutputCFunction(double (*fn)(double), double x) {
    double y = fn(x);
    std::cout << std::to_string(y) << std::endl;
}

void Foo::printOutputCppMemberFunction(int (Foo::*fn)(int, int), Foo* foo, int i, int j) {
    int k = (foo->*fn)(i, j);
    std::cout << k << std::endl;
}

void Foo::printOutputCppMemberFunctionSelf() {
    printOutputCppMemberFunction(&Foo::add, this, 7, 3);
}
```



## Domain slicing

The Game of Life simulation can be distributed in many different ways. Since it is a cellular automata, every cell is updated from *t_0* -> *t_1* fully independently. It only needs the states from its direct neighbors. This code used the domain slicing approach, which can easily be applied to other tasks, like image processing. The domain is divided horizontally in row batches. Each slice is then sent to another thread where its new state is computed. After the computation, the threads are joined and the new state is set as the current state (swap buffers). To prevent excessive memory allocation and destruction for each time step, only the memory range of the slice is given to the threads. All threads share the same memory block. This is fine, since they read the current state from buffer 1 and update only their own part in buffer 2.
