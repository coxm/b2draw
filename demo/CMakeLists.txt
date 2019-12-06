if(NOT NO_DEMO)
	find_package(SDL2 REQUIRED)

	add_executable(b2draw-demo
		"${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/util/gl.cpp")
	target_link_libraries(b2draw-demo PUBLIC
		b2draw::b2draw Box2D::Box2D ${SDL2_LIBRARIES})
endif()
