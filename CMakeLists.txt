cmake_minimum_required(VERSION 3.1)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)


set(PROJECT_NAME b2draw)
project(${PROJECT_NAME} LANGUAGES CXX VERSION 1.0.0)


# Compiler flags.
list(APPEND CXX_FLAGS_BASE
	-Werror -std=c++1z -Wall -Weffc++ -Wold-style-cast -Woverloaded-virtual
	-Wno-int-conversions -Wsign-promo -Wno-pmf-conversions
)
if(${CMAKE_COMPILER_IS_GNUCXX})
	list(APPEND CXX_FLAGS_BASE -Weffc++)
endif()
if(CMAKE_BUILD_TYPE EQUAL "Debug")
	list(APPEND CXX_FLAGS_BASE -O0 -ggdb3)
endif()


file(GLOB CPP_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

include_directories("${CMAKE_SOURCE_DIR}/include")


# Dependencies.
if(BOX2D_INCLUDE_DIR)
	include_directories(SYSTEM ${BOX2D_INCLUDE_DIR})
endif()
if(GLM_INCLUDE_DIR)
	include_directories(SYSTEM ${GLM_INCLUDE_DIR})
endif()
find_library(LIB_BOX2D Box2D PATHS "${BOX2D_LIBRARY_DIR}")
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
set(DEPENDENCY_LIBRARIES ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${LIB_BOX2D})


# Targets
add_library(${PROJECT_NAME} SHARED ${CPP_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION})
target_link_libraries(${PROJECT_NAME} ${DEPENDENCY_LIBRARIES})
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)


if(NOT NO_DEMO)
	add_subdirectory(demo)
endif()
