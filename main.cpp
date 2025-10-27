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
#include "src/menu/Menu.h"
#include "src/ui/Screen.h"
#include "src/utils/GameState.h"
#include "src/utils/Log.h"

using path = std::filesystem::path;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLFWwindow* createAndConfigureWindow(Screen& screen, bool foolscrean = false);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, PathPlayerMover& playerMover, Menu& menu, GameState& gameState);
void calculateDelta();

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = 400, lastY = 300;
bool firstMouse = false;
Camera camera = Camera{glm::vec3(0.0f, 2.0f, 3.0f) };
glm::vec3 lightPos{1.2f, 1.0f, 2.0f};
constexpr glm::vec3 whiteColor{1.0f, 1.0f, 1.0f};
float lastClickedAt = 0.0f;
float clickDeadTime = 0.25f;

int main() {

    // Create screen
    Screen screen{800, 600};

    // create and configure window
    GLFWwindow* window = createAndConfigureWindow(screen);
    if (window == nullptr) {
        return -1;
    }

    // Sound engine
    // ----------------
    SoundManager soundManager;

    // Render instantiations
    // ---------------------
    TextRenderer textRenderer(screen);
    EntityRenderer entityRenderer(camera, screen);
    TerrainRenderer terrainRenderer(camera, entityRenderer, screen);
    SkyboxRenderer skyboxRenderer(camera, screen);
    StaticShapeRenderer staticShapeRenderer{};
    UiRenderer uiRenderer{textRenderer, staticShapeRenderer};
    Skybox skybox{"cloudy"};
    Terrain terrain(
        data_dir() /= path("resources/images/heightmaps/heightmap.png"),
        data_dir() /= path("resources/images/blendMap4.png")
    );
    Fps fps;
    GameState gameState{soundManager};
    Menu menu{gameState, uiRenderer, window, screen};

    // Player related code
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

    Log::logInfo("Starting game loop");
    while (!glfwWindowShouldClose(window)) {
        // process input from the keyboard & mouse
        processInput(window, playerMover, menu, gameState);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // delta calculation
        calculateDelta();

        // fps calculation
        fps.tick();
        if (gameState.isInMenuAndGameDidNotStart() || gameState.isInMenuAndGameAlreadyStarted()) {
            // enable mouse movement
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            menu.Render();
        } else {
            // disable mouse movement
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            // input
            // -----
            playerMover.move(deltaTime);
            player.UpdateCameraPose();
            camera.tick(deltaTime * 4.0f);

            skyboxRenderer.render(skybox);
            terrainRenderer.render(terrain);
            entityRenderer.render(player);

            textRenderer.RenderText("player x:" + std::to_string(player.GetPosition().x) + " y:" + std::to_string(player.GetPosition().y) + " z:" + std::to_string(player.GetPosition().z), 25.0f, 25.0f, 0.25f, whiteColor);
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

void processInput(GLFWwindow* window, PathPlayerMover& playerMover, Menu& menu, GameState& gameState)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        menu.KeyboardEscapePressed();
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        playerMover.Jump();
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xPosition, yPosition;
        glfwGetCursorPos(window, &xPosition, &yPosition);

        // at least 250ms passed since last click
        if (glfwGetTime() - lastClickedAt > clickDeadTime) {
            menu.MouseButtonLeftClicked(xPosition, yPosition);
            lastClickedAt = glfwGetTime();
        }
    }
    if (gameState.isInMenuAndGameDidNotStart() || gameState.isInMenuAndGameAlreadyStarted()) {
        double xPosition, yPosition;
        glfwGetCursorPos(window, &xPosition, &yPosition);
        // todo: implement 250ms delay
        menu.MouseHovered(xPosition, yPosition);
    }
}

void calculateDelta() {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}
