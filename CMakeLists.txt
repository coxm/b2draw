#---- CMake configuration ----#
cmake_minimum_required (VERSION 3.1)
set (CMAKE_DISABLE_SOURCE_CHANGES ON)
set (CMAKE_DISABLE_IN_SOURCE_BUILD ON)


#---- Constants ----#
set (PROJECT_NAME b2draw)
project (${PROJECT_NAME})

# Load dependency requirements.
include (dependencies/versions.cmake)
include (dependencies/locations.cmake)

# Project directories.
set (PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set (CONFIG_HEADERS_DIR "${CMAKE_BINARY_DIR}/config_headers")
set (PROJECT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

set (PROJECT_LIB ${PROJECT_NAME})

set (VERSION_MAJOR 0)
set (VERSION_MINOR 0)
set (VERSION_PATCH 0)
set (PROJECT_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

configure_file (
	"${PROJECT_INCLUDE_DIR}/constants.h.in"
	"${CONFIG_HEADERS_DIR}/constants.h"
)


#---- Compiler configuration and preprocessor variables ----#
set (EXTRA_CXX_FLAGS
	"-Werror -std=c++1y -Wall -Weffc++ -Wold-style-cast -Woverloaded-virtual -Wno-int-conversions -Wsign-promo -D_GLIBCXX_USE_CXX11_ABI=0"
)

# TODO: Check the gdb version (how?) and pick the appropriate flags
# accordingly.
# See http://stackoverflow.com/questions/12595631/debugging-with-gdb-on-a-program-with-no-optimization-but-still-there-is-no-symbo#12595773 
# set (CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb ${EXTRA_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3 -gdwarf-2 ${EXTRA_CXX_FLAGS}")

if (${CMAKE_COMPILER_IS_GNUCXX})
	set (EXTRA_CXX_FLAGS "${EXTRA_CXX_FLAGS} -Wno-pmf-conversions")
endif ()
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_FLAGS}")


#---- File and library detection ----#
file (GLOB CPP_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

include_directories ("${CONFIG_HEADERS_DIR}" "${PROJECT_INCLUDE_DIR}")
include_directories (
	SYSTEM
	${BOX2D_INCLUDE_DIR}
	${OGL_INCLUDE_DIR}
	${MACROS_INCLUDE_DIR}
	${LOGGING_INCLUDE_DIR}
)

find_library (LIB_OGL ogl PATHS "${OGL_LIBRARY_DIR}")
find_library (LIB_BOX2D Box2D PATHS "${BOX2D_LIBRARY_DIR}")
find_package (OpenGL REQUIRED)
find_package (GLEW REQUIRED)

set (
	DEPENDENCY_LIBRARIES
	${OPENGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${LIB_BOX2D}
	${LIB_OGL}
)


# Enforce dependency requirements.
include (dependencies/enforce.cmake)


#---- Targets ----#
add_library (${PROJECT_LIB} SHARED ${CPP_SOURCES})
set_target_properties (${PROJECT_LIB} PROPERTIES SOVERSION ${PROJECT_VERSION})
target_link_libraries (${PROJECT_LIB} ${DEPENDENCY_LIBRARIES})
install (TARGETS ${PROJECT_LIB} LIBRARY DESTINATION lib)


add_subdirectory (demo)
