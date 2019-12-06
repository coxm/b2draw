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


## Building the library
Configure as with a normal CMake project. CMake package finders will be used
for locating Box2D, GLEW, OpenGL and SDL2 where required.

### Example
Assuming dependencies are installed in places where the compiler will find
them:

    mkdir build
    cd build
    cmake ..


## Building the demo
To run the demo, build as above but ensure to define `b2draw_BUILD_DEMO`, and
that GLM and SDL2 can be found. Once built, run `$BUILD_DIR/demo/demo`, where
`BUILD_DIR` is the path to your build directory.


### Example

    mkdir build
    cd build
    cmake -Db2draw_BUILD_DEMO=ON ..
