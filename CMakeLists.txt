cmake_minimum_required(VERSION 3.28)
project(mygame)

set(CMAKE_CXX_STANDARD 17)

# Set GLFW directory path
set(GLFW_DIR "/Users/vladino/CLionProjects/mygame/glfw")
set(CMAKE_CXX_FLAGS "-framework Cocoa -framework IOKit")

# Include GLFW
include_directories(${GLFW_DIR}/include )
link_directories(${GLFW_DIR}/lib-arm64)

# Find OpenGL
find_package(OpenGL REQUIRED)
if (NOT OpenGL_FOUND)
    message("ERROR: OpenGL not found")
endif()

include(FetchContent)

FetchContent_Declare(
        glm
        GIT_REPOSITORY	https://github.com/g-truc/glm.git
        GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)

FetchContent_MakeAvailable(glm)

#target_link_libraries(main PRIVATE glm::glm)

add_executable(mygame main.cpp
        glad.c
        shader.h
        customstb.cpp
        add.cpp
        camera.h
        terrain.cpp
        terrain.h
)

target_link_libraries(mygame glfw3 glm::glm)
