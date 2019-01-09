if(NOT NO_DEMO)
	message(STATUS "Building demo...")

	find_library(LIB_SDL SDL2)
	if(LIB_SDL)
		file(
			GLOB
			DEMO_CPP_SOURCES
			"${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/util/*.cpp"
		)
		set(
			DEMO_LIBRARIES
			${DEPENDENCY_LIBRARIES}
			${PROJECT_NAME}
			${LIB_SDL}
		)
		add_executable(demo ${DEMO_CPP_SOURCES})
		target_link_libraries(demo ${DEMO_LIBRARIES})
	else()
		message(STATUS "Unable to find libSDL2. Not building demo.")
	endif()
endif()
