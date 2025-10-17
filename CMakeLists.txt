cmake_minimum_required(VERSION 3.28)
cmake_policy(VERSION 3.5...3.30)
project(mygame)

set(CMAKE_CXX_STANDARD 17)

find_library(COCOA_FRAMEWORK Cocoa)
find_library(IOKIT_FRAMEWORK IOKit)
mark_as_advanced(COCOA_FRAMEWORK IOKIT_FRAMEWORK)

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

# Download and include Assimp
FetchContent_Declare(assimp GIT_REPOSITORY https://github.com/assimp/assimp.git)
FetchContent_MakeAvailable(assimp)

# Download and include lunasvg
FetchContent_Declare(lunasvg GIT_REPOSITORY https://github.com/sammycage/lunasvg.git GIT_TAG master)
FetchContent_MakeAvailable(lunasvg)

cmake_path(SET DATA_DIR "${PROJECT_SOURCE_DIR}")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/data_dir.h.in" "${CMAKE_CURRENT_BINARY_DIR}/src/data_dir.h")

# Download and include miniaudio
file(DOWNLOAD
    https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h
    ${CMAKE_CURRENT_BINARY_DIR}/external/miniaudio.h
    TLS_VERIFY ON
)

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
        src/images/Image.cpp
        src/images/Image.h
        src/models/Mesh.cpp
        src/models/Mesh.h
        src/models/Model.cpp
        src/models/Model.h
        src/models/EntityRenderer.cpp
        src/models/EntityRenderer.h
        src/models/ModelGenerator.cpp
        src/models/ModelGenerator.h
        src/terrain/EntitiesHolder.h
        src/objects/Entity.cpp
        src/objects/Entity.h
        src/terrain/Grasses.cpp
        src/terrain/Grasses.h
        src/objects/Player.cpp
        src/objects/Player.h
        src/objects/movers/RandomPlayerMover.cpp
        src/objects/movers/RandomPlayerMover.h
        src/skybox/Skybox.cpp
        src/skybox/Skybox.h
        src/skybox/SkyboxRenderer.cpp
        src/skybox/SkyboxRenderer.h
        src/pathing/Path.cpp
        src/pathing/Path.h
        src/objects/movers/PathPlayerMover.cpp
        src/objects/movers/PathPlayerMover.h
        src/utils/Log.h
        src/utils/Log.cpp
        src/pathing/SvgDElementPathParser.h
        src/utils/GameState.cpp
        src/utils/GameState.h
        src/menu/Menu.cpp
        src/menu/Menu.h
        src/text/TextProportion.cpp
        src/text/TextProportion.h
        src/text/Character.h
        src/models/StaticShape.cpp
        src/models/StaticShape.h
        src/ui/Element.cpp
        src/ui/Element.h
        src/ui/UiRenderer.cpp
        src/ui/UiRenderer.h
        src/models/StaticShapeRenderer.cpp
        src/models/StaticShapeRenderer.h
        src/ui/Screen.h
        src/audio/audio.cpp
        src/audio/SoundManager.cpp
        src/audio/SoundManager.h
)

target_include_directories(mygame PRIVATE ${stb_SOURCE_DIR} "${CMAKE_CURRENT_BINARY_DIR}/src" "${CMAKE_CURRENT_BINARY_DIR}/external")

target_link_libraries(mygame glfw glm::glm freetype glad assimp lunasvg::lunasvg)

