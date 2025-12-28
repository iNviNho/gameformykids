#include <iostream>
#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "src/camera/camera.h"
#include "src/shaders/shader.h"
#include "src/fps/Fps.h"
#include "src/models/Model.h"
#include "src/models/EntityRenderer.h"
#include "src/objects/Player.h"
#include "src/objects/movers/PathPlayerMover.h"
#include "src/skybox/SkyboxRenderer.h"
#include "src/terrain/Terrain.h"
#include "src/terrain/TerrainRenderer.h"
#include "src/text/TextRenderer.h"
#include <data_dir.h>

#include "src/audio/SoundManager.h"
#include "src/gameedit/SceneModifier.h"
#include "src/menu/Menu.h"
#include "src/models/ModelsHolder.h"
#include "src/storage/LocalStorage.h"
#include "src/terrain/DoodadsLoader.h"
#include "src/ui/Screen.h"
#include "src/utils/GameState.h"
#include "src/utils/Log.h"

using path = std::filesystem::path;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLFWwindow* createAndConfigureWindow(Screen& screen, bool fullscreen = false);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window, PathPlayerMover& playerMover, Menu& menu, GameState& gameState, SceneModifier& sceneModifier);
void calculateDelta();
bool smallDelayPassed();

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = 400, lastY = 300;
bool firstMouse = false;
Camera camera = Camera{glm::vec3(0.0f, 2.0f, 3.0f) };
glm::vec3 lightPos{1.2f, 1.0f, 2.0f};
constexpr glm::vec3 whiteColor{1.0f, 1.0f, 1.0f};
float lastClickedAt = 0.0f;
float lastPressetAt = 0.0f;
float deadTime = 0.25f;

int main() {

    // Create screen
    Screen screen{800, 600};

    // create and configure window
    GLFWwindow* window = createAndConfigureWindow(screen);
    if (window == nullptr) {
        return -1;
    }

    // Performance
    // -----------
    Fps fps;

    // Sound engine
    // ----------------
    SoundManager soundManager;

    // Game state
    // ----------------
    GameState gameState{soundManager};

    // Render instantiations
    // ---------------------
    TextRenderer textRenderer(screen);
    EntityRenderer entityRenderer(camera, screen);
    TerrainRenderer terrainRenderer(camera, entityRenderer, screen);
    SkyboxRenderer skyboxRenderer(camera, screen);
    StaticShapeRenderer staticShapeRenderer{};
    UiRenderer uiRenderer{textRenderer, staticShapeRenderer};

    // Models instantiations
    // ---------------------
    ModelsHolder modelsHolder{};
    modelsHolder.LoadModels();
    EntitiesHolder doodads{};
    LocalStorage storageForDoodads{data_dir() /= path("resources/map/doodads.txt")};
    // load persisted doodads
    DoodadsLoader::LoadDoodads(
        modelsHolder,
        storageForDoodads,
        doodads
    );
    Skybox skybox{"cloudy"};
    Terrain terrain(
        data_dir() /= path("resources/images/heightmaps/heightmap.png"),
        data_dir() /= path("resources/images/blendMap4.png")
    );

    // Menu
    // -----------------
    Menu menu{gameState, uiRenderer, window, screen};

    // Game edit
    // -----------------
    SceneModifier sceneModifier{camera, terrain, doodads, storageForDoodads, modelsHolder};
    StaticShape crosshair{
        data_dir() /= path("resources/images/pointers/pointer.png")
    };
    crosshair.SetScale(glm::vec2{0.05f, 0.05f});

    // Player
    // -------------------
    std::shared_ptr<Model> wolf = std::make_shared<Model>(data_dir() /= path("resources/objects/animals/wolf2/Wolf_One_obj.obj"));
    Player player(
        camera,
        terrain,
        wolf,
        glm::vec3(0.0f, 0.0f, 0.0f)
    );
    PathPlayerMover playerMover(player, terrain);

    glEnable(GL_DEPTH_TEST);

    // enabling this will draw only lines
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    Log::logInfo("Starting game loop");
    while (!glfwWindowShouldClose(window)) {
        // process input from the keyboard & mouse
        processInput(window, playerMover, menu, gameState, sceneModifier);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // delta calculation
        calculateDelta();

        // fps calculation
        fps.tick();

        // We have 2 different types of master modes:
        // first = IN MENU
        if (gameState.isInMenuAndGameDidNotStart() || gameState.isInMenuAndGameAlreadyStarted()) {
            // enable mouse movement
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            menu.Render();
        // second = IN GAME
        } else {
            // ****************************
            // while in game, we have shared functionality and 2 different states
            // ****************************
            // first = SHARED FUNCTIONALITY
            // disable mouse movement
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            // camera ticks
            camera.tick(deltaTime);
            // renderers
            skyboxRenderer.render(skybox);
            terrainRenderer.render(terrain);
            entityRenderer.render(player);
            for (const Entity& entity : doodads.GetEntities()) {
                entityRenderer.render(entity);
            }

            // ****************************
            // second = EDIT MODE
            if (gameState.isGameEditModeEnabled()) {
                // we render crosshair in the middle of the screen
                staticShapeRenderer.Render(crosshair);
                // render selected entity preview
                entityRenderer.render(
                    sceneModifier.GetSelectedEntityPreviewEntity()
                );

                // misc texts
                textRenderer.RenderText("selected item to add: " + sceneModifier.GetSelectedEntityName(), 25.0f, 10.0f, 0.25f, whiteColor);
                textRenderer.RenderText("selected item scale : " + std::to_string(sceneModifier.GetScale()), 25.0f, 30.0f, 0.25f, whiteColor);
                textRenderer.RenderText("selected rotation x:" + std::to_string(sceneModifier.GetRotation().x) + " y:" + std::to_string(sceneModifier.GetRotation().y) + " z:" + std::to_string(sceneModifier.GetRotation().z), 25.0f, 50.0f, 0.25f, whiteColor);

            // ****************************
            // third = GAME MODE
            } else {
                // move player
                playerMover.move(deltaTime);
                player.UpdateCameraPose();

                // misc texts
                textRenderer.RenderText("player x:" + std::to_string(player.GetPosition().x) + " y:" + std::to_string(player.GetPosition().y) + " z:" + std::to_string(player.GetPosition().z), 25.0f, 25.0f, 0.25f, whiteColor);
            }
        }
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    return 0;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


GLFWwindow* createAndConfigureWindow(Screen& screen, bool fullscreen) {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window;
    if (fullscreen) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        screen.Resize(mode->width, mode->height);
        window = glfwCreateWindow(screen.GetWidth(), screen.GetHeight(), "gameformykids", monitor, nullptr);
    } else {
        window = glfwCreateWindow(screen.GetWidth(), screen.GetHeight(), "gameformykids", NULL, nullptr);
    }

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    // could be flipped (depends on the mouse)
    float yoffset = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void processInput(GLFWwindow* window, PathPlayerMover& playerMover, Menu& menu, GameState& gameState, SceneModifier& sceneModifier)
{
    /**************
     * START OF KEY TO ACTION MAPPINGS
     *************/
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        menu.KeyboardEscapePressed();
    }
    if (gameState.isGameEditModeEnabled()) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.ProcessKeyboard(FORWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.ProcessKeyboard(LEFT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.ProcessKeyboard(RIGHT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera.ProcessKeyboard(UP, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            camera.ProcessKeyboard(DOWN, deltaTime);
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            if (smallDelayPassed()) {
                sceneModifier.ChangeSelectedEntityName();
            }
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            sceneModifier.decreaseRotationX();
        } else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            sceneModifier.increaseRotationX();
        }

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            sceneModifier.decreaseRotationY();
        } else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            sceneModifier.increaseRotationY();
        }

        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            sceneModifier.decreaseRotationZ();
        } else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
            sceneModifier.increaseRotationZ();
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            if (smallDelayPassed()) {
                sceneModifier.decreaseScale();
            }
        } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            if (smallDelayPassed()) {
                sceneModifier.increaseScale();
            }
        }
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        playerMover.Jump();
    }
    /**************
     * END OF KEY TO ACTION MAPPINGS
     *************/

    /**************
     * START OF CLICK TO ACTION MAPPINGS
     *************/
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xPosition, yPosition;
        glfwGetCursorPos(window, &xPosition, &yPosition);
        // we differentiate between click in menu and click in the game
        // in menu we process clicks to menu items
        if (gameState.isInMenuAndGameDidNotStart() || gameState.isInMenuAndGameAlreadyStarted()) {
            if (smallDelayPassed()) {
                menu.MouseButtonLeftClicked(xPosition, yPosition);
            }
        // in game we only handle clicks in game edit mode
        } else {
            if (smallDelayPassed() && gameState.isGameEditModeEnabled()) {
                sceneModifier.placeObject();
            }
        }
    }
    /**************
     * END OF CLICK TO ACTION MAPPINGS
     *************/

    /**************
     * START OF HOVER TO ACTION MAPPINGS
     *************/
    if (gameState.isInMenuAndGameDidNotStart() || gameState.isInMenuAndGameAlreadyStarted()) {
        double xPosition, yPosition;
        glfwGetCursorPos(window, &xPosition, &yPosition);
        // todo: extend smallDelayPassed with custom action items & delay so that
        // usage of methods at various places don't interfere with each other
        menu.MouseHovered(xPosition, yPosition);
    }
    /**************
     * END OF HOVER TO ACTION MAPPINGS
     *************/
}

bool smallDelayPassed() {
    if (glfwGetTime() - lastPressetAt > deadTime) {
        lastPressetAt = glfwGetTime();
        return true;
    }
    return false;
}

void calculateDelta() {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}
