if (NOT DEFINED BOX2D_INCLUDE_DIR)
	message (STATUS "Note: BOX2D_INCLUDE_DIR is not defined")
endif ()
if (NOT LIB_BOX2D)
	message (FATAL_ERROR "No libBox2D in ${BOX2D_LIBRARY_DIR}")
endif ()


if (NOT DEFINED MACROS_INCLUDE_DIR)
	message (STATUS "Note: MACROS_INCLUDE_DIR is not defined")
endif ()


if (NOT DEFINED LOGGING_INCLUDE_DIR)
	message (STATUS "Note: LOGGING_INCLUDE_DIR is not defined")
endif ()


if (NOT DEFINED OGL_INCLUDE_DIR)
	message (STATUS "Note: OGL_INCLUDE_DIR is not defined")
endif ()
if (NOT LIB_OGL)
	message (FATAL_ERROR "No libogl in ${OGL_LIBRARY_DIR}")
endif ()
