cmake_minimum_required(VERSION 3.28)
cmake_policy(VERSION 3.5...3.30)
project(gameformykids)

find_library(COCOA_FRAMEWORK Cocoa)
find_library(IOKIT_FRAMEWORK IOKit)
mark_as_advanced(COCOA_FRAMEWORK IOKIT_FRAMEWORK)


# Find OpenGL
find_package(OpenGL REQUIRED)

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
if(TARGET assimp AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(assimp PRIVATE
        -Wno-error
        -Wno-unsafe-buffer-usage
        -Wno-unsafe-buffer-usage-in-libc-call
    )
endif()

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

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_executable(gameformykids ${GAME_SOURCES})

target_include_directories(gameformykids
    PRIVATE ${stb_SOURCE_DIR}
    "${CMAKE_CURRENT_BINARY_DIR}/src"
    "${CMAKE_CURRENT_BINARY_DIR}/external"
)

target_link_libraries(gameformykids
    PRIVATE
        glfw
        glm::glm
        freetype
        glad
        assimp
        lunasvg::lunasvg
        OpenGL::GL
)

target_compile_features(gameformykids PRIVATE cxx_std_17)

set_target_properties(gameformykids PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "gameformykids"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.invinho.gameformykids"
)

if(APPLE)
    install(
        TARGETS gameformykids
        BUNDLE DESTINATION .
    )

else()
    install(
        TARGETS gameformykids
        RUNTIME DESTINATION .
    )

endif()

install(DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/"
    DESTINATION "./gameformykids.app/Contents/Resources"
)
