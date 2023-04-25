set(LIB_INCLUDE_DIRECTORIES "")

# OpenGL
find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
	MESSAGE("OpenGL Correctly Found at: ${OPENGL_INCLUDE_DIR}")
	include_directories(${OPENGL_INCLUDE_DIR})
else ()
	MESSAGE("OpenGL environment missing")
endif (OPENGL_FOUND)

# SDL3
set(SDL_TEST OFF)

find_package(SDL2 REQUIRED)
#target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} PUBLIC SDL2)

