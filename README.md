# b2Draw
An OpenGL implementation of Box2D's DebugDraw.

## Dependencies
For building and using the library, install:
-   [Box2D](box2d.org) v2.3.2 ([github](https://github.com/erincatto/Box2D));
-   a modern OpenGL implementation;
-   GLEW.

Additionally, to build and run the demo, you'll need:
-   [SDL2](https://www.libsdl.org/);
-   [GLM](https://glm.g-truc.net/0.9.8/index.html) ([github](https://github.com/g-truc/glm)).


## Building
Configure as with a normal CMake project. CMake package finders will be used
for locating Box2D, GLEW, OpenGL and SDL2 where required.

### Example
Assuming dependencies are installed in places where the compiler will find
them:

    mkdir build
    cd build
    cmake ..
    cmake --build .

You can then optionally install by running e.g. `make install` (as usual, the
install prefix can be modified by setting `-DCMAKE_INSTALL_PREFIX` when
invoking `cmake`; see the [CMake documentation](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html)
for details).


## Consuming
### CMake projects
Either install as above and use `find_package(b2draw)`, or if, for example,
using Git submodules, you can `add_subdirectory(path/to/b2draw)`. In either
case, linking against the `b2draw::b2draw` library should provide dependants
with the correct include dirctories and link libraries.

### Non-CMake projects
Either install as above, or build in-place to include in your build; the
details will depend on your toolchain.

### In the source
Create a debug draw:

    // Configuration...
    DebugDraw debugDraw(...);
    debugDraw.SetFlags(0xff); // Draw everything.
    b2World world;
    world.SetDebugDraw(&debugDraw);

When you update the Box2D world, make sure to buffer the debug draw data:

    // Logic loop:
    debugDraw.Clear(); // Clears the previous geometry set.
    world.DrawDebugData(); // Adds world geometry to the DebugDraw.
    debugDraw.BufferData(); // Sends geometry to the GPU.

Finally, when rendering, call `DebugDraw::Render()`:

    // Render loop:
    debugDraw.Render();


## Demo
To run the demo, build as above but ensure to define `b2draw_BUILD_DEMO`, and
that GLM and SDL2 can be found. Once built, run `$BUILD_DIR/demo/demo`, where
`BUILD_DIR` is the path to your build directory.


### Example

    mkdir build
    cd build
    cmake -Db2draw_BUILD_DEMO=ON ..
