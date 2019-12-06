cmake_minimum_required(VERSION 3.7)
project(b2draw VERSION 4.0.0 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

option(b2draw_BUILD_DEMO "Build the demo application" OFF)


find_package(Box2D 2.3.1 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW 2.0 EXACT REQUIRED)


add_library(b2draw "src/DebugDraw.cpp" "src/PrimitiveRenderer.cpp")
add_library(b2draw::b2draw ALIAS b2draw)
set_target_properties(b2draw PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION}
	PROJECT_LABEL "b2draw"
	DEBUG_POSTFIX d)
target_include_directories(b2draw PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(b2draw PUBLIC
	Box2D::Box2D ${Box2D_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
target_compile_options(b2draw PRIVATE
	$<$<CXX_COMPILER_ID:GNU>:-Wall -Weffc++ -Werror -Wshadow -Wold-style-cast -Woverloaded-virtual>)


configure_package_config_file(
	"${PROJECT_SOURCE_DIR}/b2draw-config.cmake.in"
	"${PROJECT_BINARY_DIR}/b2draw-config.cmake"
	INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/b2draw")
write_basic_package_version_file(
	"${PROJECT_BINARY_DIR}/b2draw-config-version.cmake"
	COMPATIBILITY AnyNewerVersion)
configure_file(b2draw.pc.in b2draw.pc @ONLY)


install(TARGETS b2draw
	EXPORT b2draw-targets
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	FILES_MATCHING PATTERN "*.h")
install(EXPORT b2draw-targets
	DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/b2draw")
install(FILES
	"${PROJECT_BINARY_DIR}/b2draw-config.cmake"
	"${PROJECT_BINARY_DIR}/b2draw-config-version.cmake"
	DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/b2draw")
install(FILES "${PROJECT_BINARY_DIR}/b2draw.pc"
	DESTINATION ${CMAKE_INSTALL_DATADIR}/pkgconfig)


if(b2draw_BUILD_DEMO)
	add_subdirectory(demo)
endif()
