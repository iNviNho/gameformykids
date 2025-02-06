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

# Download and include FreeType
FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG VER-2-10-4
)

FetchContent_MakeAvailable(freetype)


# Download stb_image
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG        master
)

FetchContent_MakeAvailable(stb)

add_executable(mygame main.cpp
        src/shaders/shader.h
        src/stbimage/stb.cpp
        src/camera/camera.h
        src/terrain/Terrain.cpp
        src/terrain/Terrain.h
        src/glad/glad.c
)

target_include_directories(mygame PRIVATE
        ${GLFW_DIR}/include
        ${stb_SOURCE_DIR}
)

target_link_libraries(mygame glfw3 glm::glm freetype)
