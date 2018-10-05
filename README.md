# b2Draw
An OpenGL implementation of Box2D's DebugDraw.

## Quick start

### Dependencies
You'll need the following dependencies. In each case, if cloning the source or
installing to a non-standard location, you can define the source and library
locations when configuring CMake.

For building and using the library:
-   [Box2D](box2d.org) v2.3.2 ([github](https://github.com/erincatto/Box2D));
-   a modern OpenGL implementation;
-   GLEW.

Additionally, to build and run the demo, you'll need
-   [SDL2](https://www.libsdl.org/);
-   [GLM](https://glm.g-truc.net/0.9.8/index.html) ([github](https://github.com/g-truc/glm)).


### Configure CMake
Configure as usual, defining any dependency locations if required (use the form
`<LIBRARY_NAME>_INCLUDE_DIR` for headers and `<LIBRARY_NAME>_LIBRARY_DIR` for
libraries.

#### Simple example
Assuming dependencies are installed in places where the compiler will find
them:

    mkdir build
    cd build
    cmake ..

#### Custom locations example
For example, to create a debug build with dependencies installed somewhere
unusual:

    mkdir build && cd build
    cmake \
        -DCMAKE_BUILD_TYPE=Debug \
        -DBOX2D_INCLUDE_DIR=/box2d/include/dir/ \
        -DBOX2D_LIBRARY_DIR=/box2d/library/dir/ \
        -DGLM_INCLUDE_DIR=/path/to/glm/ \
        ..


## Demo
To run the demo, build as above but ensure to define `BUILD_DEMO`, and that GLM
and SDL2 can be found. Once built, run `$BUILD_DIR/demo/demo`, where
`BUILD_DIR` is the path to your build directory.

Once the demo is running, you'll have to hit 's' (for "step") to step and
render the simulation.
