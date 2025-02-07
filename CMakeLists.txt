cmake_minimum_required(VERSION 3.28)
project(mygame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-framework Cocoa -framework IOKit")

# Find OpenGL
find_package(OpenGL REQUIRED)
if (NOT OpenGL_FOUND)
    message("ERROR: OpenGL not found")
endif()

include(FetchContent)

# Download and include GLFW
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw)
FetchContent_MakeAvailable(glfw)

# Download and include GLAD
FetchContent_Declare(glad GIT_REPOSITORY https://github.com/Dav1dde/glad.git)
FetchContent_MakeAvailable(glad)

# Download and include GLM
FetchContent_Declare(glm GIT_REPOSITORY	https://github.com/g-truc/glm.git)
FetchContent_MakeAvailable(glm)

# Download and include FreeType
FetchContent_Declare(freetype GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git GIT_TAG VER-2-10-4)
FetchContent_MakeAvailable(freetype)

# Download stb_image
FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb.git)
FetchContent_MakeAvailable(stb)

add_executable(mygame main.cpp
        src/shaders/shader.h
        src/stbimage/stb.cpp
        src/camera/camera.h
        src/terrain/Terrain.cpp
        src/terrain/Terrain.h
        src/text/TextRenderer.cpp
        src/text/TextRenderer.h
        src/terrain/TerrainRenderer.cpp
        src/terrain/TerrainRenderer.h
        src/textures/TextureLoader.h
        src/textures/TextureLoader.h
        src/fps/Fps.cpp
        src/fps/Fps.h
)

target_include_directories(mygame PRIVATE ${stb_SOURCE_DIR})

target_link_libraries(mygame glfw glm::glm freetype glad)
