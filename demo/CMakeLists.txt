if(NOT NO_DEMO)
	find_package(SDL2 REQUIRED)
	find_package(glm REQUIRED)

	add_executable(b2draw-demo
		"${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/util/gl.cpp")
	set_target_properties(b2draw-demo PROPERTIES OUTPUT_NAME demo)
	target_include_directories(b2draw-demo PUBLIC ${GLM_INCLUDE_DIRS})
	target_link_libraries(b2draw-demo PUBLIC
		b2draw::b2draw Box2D::Box2D ${SDL2_LIBRARIES})
endif()
