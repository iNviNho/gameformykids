#include <iostream>
#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera/camera.h"
#include "fps/Fps.h"
#include "models/EntityRenderer.h"
#include "objects/Player.h"
#include "objects/movers/PathPlayerMover.h"
#include "skybox/SkyboxRenderer.h"
#include "terrain/Terrain.h"
#include "terrain/TerrainRenderer.h"
#include "text/TextRenderer.h"
#include <data_dir.h>
#include <string>

#include "audio/SoundManager.h"
#include "gameedit/SceneModifier.h"
#include "menu/Menu.h"
#include "models/ModelsHolder.h"
#include "models/StaticModel.h"
#include "storage/LocalStorage.h"
#include "terrain/DoodadsLoader.h"
#include "ui/Screen.h"
#include "utils/GameState.h"
#include "utils/Log.h"

using path = std::filesystem::path;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLFWwindow* createAndConfigureWindow(Screen& screen, bool fullscreen = false);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window, PathPlayerMover& playerMover, Menu& menu, GameState& gameState, SceneModifier& sceneModifier);
void calculateDelta();
bool smallDelayPassed();
bool extraSmallDelayPassed();

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = 400, lastY = 300;
bool firstMouse = false;
Camera camera = Camera{glm::vec3(0.0f, 2.0f, 3.0f) };
glm::vec3 lightPos{1.2f, 1.0f, 2.0f};
constexpr glm::vec3 whiteColor{1.0f, 1.0f, 1.0f};
float lastClickedAt = 0.0f;
float lastPressetAtForExtraSmallDelay = 0.0f; 
float lastPressetAt = 0.0f;
float extraSmallDeadTime = 0.1f;
float deadTime = 0.25f;
long startTimeInMillis = glfwGetTime() * 1000;

int main() {

    // Create screen
    Screen screen{};

    // Performance
    // -----------
    Fps fps;

    // Load settings
    // ----------------
    LocalStorage settingsStorage{data_dir() /= path("settings.txt")};

    // Sound engine
    // ----------------
    bool soundEnabled = settingsStorage.getOne("soundenabled", "0") == "0" ? false : true;
    SoundManager soundManager{soundEnabled};

    // Game state
    // ----------------
    GameState gameState{soundManager};

    std::string editModeValue = settingsStorage.getOne("editmode", "0");
    gameState.setGameEditMode(editModeValue == "1");
    std::string polygonMode = settingsStorage.getOne("polygonmode", "0");
    if (polygonMode == "1") {
        // enabling this will draw only lines
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    
    // Create and configure window
    std::string fullscreenMode = settingsStorage.getOne("fullscreen", "0");
    GLFWwindow* window = createAndConfigureWindow(
        screen,
        fullscreenMode == "1"
    );
    if (window == nullptr) {
        return -1;
    }

    // Render instantiations
    // ---------------------
    TextRenderer textRenderer(screen);
    EntityRenderer entityRenderer(camera, screen);
    // new line
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
        data_dir() /= path("resources/images/blendMap4.png")
    );
    
    // Player
    // -------------------
    std::shared_ptr<StaticModel> wolf = std::make_shared<StaticModel>(data_dir() /= path("resources/objects/animals/wolf2/Wolf_One_obj.obj"));
    Player player(
        camera,
        terrain,
        wolf,
        glm::vec3(0.0f, 0.0f, 0.0f)
    );
    PathPlayerMover playerMover(player, terrain);


    // Menu
    // -----------------
    Menu menu{gameState, uiRenderer, window, screen, playerMover};

    // Game edit
    // -----------------
    SceneModifier sceneModifier{camera, terrain, doodads, storageForDoodads, modelsHolder};
    StaticShape placeObjectCrosshair{
        data_dir() /= path("resources/images/pointers/pointer.png")
    };
    placeObjectCrosshair.SetScale(glm::vec2{0.05f, 0.05f});
    StaticShape removeObjectCrosshair{
        data_dir() /= path("resources/images/pointers/removePointer.png")
    };
    removeObjectCrosshair.SetScale(glm::vec2{0.05f, 0.05f});


    
    glEnable(GL_DEPTH_TEST);

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
            entityRenderer.render(player, startTimeInMillis);
            for (const Entity& entity : doodads.GetEntities()) {
                entityRenderer.render(entity, startTimeInMillis);
            }

            // ****************************
            // second = EDIT MODE
            if (gameState.isGameEditModeEnabled()) {

                // do we plan to place or remove an object?
                if (glfwGetKey(window, GLFW_KEY_X) != GLFW_PRESS) {
                    // we render crosshair in the middle of the screen
                    staticShapeRenderer.Render(placeObjectCrosshair);
                    // render selected entity preview
                    entityRenderer.render(
                        sceneModifier.GetSelectedEntityPreviewEntity(),
                        startTimeInMillis
                    );
                } else {
                    // we render x in the middle of the screen
                    staticShapeRenderer.Render(removeObjectCrosshair);
                }

                // misc texts
                textRenderer.BufferText(("selected radius for increasing height: " + std::to_string(sceneModifier.getSelectedRadius())).c_str(), 25.0f, 70.0f, 0.25f, whiteColor);
                textRenderer.BufferText(("selected item: " + sceneModifier.GetSelectedEntityName()).c_str(), 25.0f, 10.0f, 0.25f, whiteColor);
                textRenderer.BufferText(("selected item scale : " + std::to_string(sceneModifier.GetScale())).c_str(), 25.0f, 30.0f, 0.25f, whiteColor);
                textRenderer.BufferText(("selected rotation x:" + std::to_string(sceneModifier.GetRotation().x) + " y:" + std::to_string(sceneModifier.GetRotation().y) + " z:" + std::to_string(sceneModifier.GetRotation().z)).c_str(), 25.0f, 50.0f, 0.25f, whiteColor);

            // ****************************
            // third = GAME MODE
            } else {
                // move player
                playerMover.move(deltaTime);
                if (!playerMover.IsPaused()) {
                    player.UpdateCameraPose();
                }
                

                // misc texts
                textRenderer.BufferText(("player x:" + std::to_string(player.GetPosition().x) + " y:" + std::to_string(player.GetPosition().y) + " z:" + std::to_string(player.GetPosition().z)).c_str(), 25.0f, 25.0f, 0.25f, whiteColor);
            }
        }
        // We render whatever is in the text buffer
        textRenderer.RenderBufferedText();
       
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
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
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
        window = glfwCreateWindow(mode->width, mode->height, "gameformykids", monitor, nullptr);
    } else {
        window = glfwCreateWindow(screen.GetWidth(), screen.GetHeight(), "gameformykids", nullptr, nullptr);
    }

    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    // Get actual window size and update Screen
    int actualWidth, actualHeight;
    glfwGetWindowSize(window, &actualWidth, &actualHeight);
    screen.Resize(actualWidth, actualHeight);
    Log::logInfo("Creating window of width: " + std::to_string(actualWidth) + " and height: " + std::to_string(actualHeight));

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // Set the Screen pointer as the window user pointer
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
    // todo: should not be possible while game is on (no game edit, no menu)
    auto xpos = static_cast<float>(xposIn);
    auto ypos = static_cast<float>(yposIn);

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


// TODO: It is too big, time to move this to separate class just handling & orchestrating inputs from the user 
void processInput(GLFWwindow* window, PathPlayerMover& playerMover, Menu& menu, GameState& gameState, SceneModifier& sceneModifier)
{
    /**************
     * START OF KEY TO ACTION MAPPINGS
     ************/
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
       
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            if (smallDelayPassed()) {
                sceneModifier.ModifySelectedRadius(-1);
            }
        } else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
            if (smallDelayPassed()) {
                sceneModifier.ModifySelectedRadius(1);
            }
        }
    } else {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            playerMover.Jump();
        }    
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && smallDelayPassed()) {
            playerMover.PauseToggle();
        }
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
                // we handle 2 types of clicks:
                // first = place object if x is not pressed
                if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
                } else if (glfwGetKey(window, GLFW_KEY_X) != GLFW_PRESS) {
                   sceneModifier.placeObject();
                // second = remove object if x is pressed
                } else {
                    sceneModifier.removeObject();
                }
            }
            if (extraSmallDelayPassed() && gameState.isGameEditModeEnabled()) {
                if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
                    sceneModifier.ModifyTerrainHeight(1);
                }            
                if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                    sceneModifier.ModifyTerrainHeight(-1);
                }
                if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
                    sceneModifier.ModifyTerrainHeight(0);
                }
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
     *
     * END OF HOVER TO ACTION MAPPINGS
     *************/
}

bool smallDelayPassed() {
    if (glfwGetTime() - lastPressetAt > deadTime) {
        lastPressetAt = static_cast<float>(glfwGetTime());
        return true;
    }
    return false;
}
bool extraSmallDelayPassed() {
    if (glfwGetTime() - lastPressetAtForExtraSmallDelay > extraSmallDeadTime) {
        lastPressetAtForExtraSmallDelay = static_cast<float>(glfwGetTime());
        return true;
    }
    return false;
}

void calculateDelta() {
    auto currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}
