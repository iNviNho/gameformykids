#include <iostream>
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
#include "src/objects/movers/RandomPlayerMover.h"
#include "src/terrain/Terrain.h"
#include "src/terrain/TerrainRenderer.h"
#include "src/text/TextRenderer.h"

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLFWwindow* createAndConfigureWindow();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void calculateDelta();

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = 400, lastY = 300;
bool firstMouse = false;
Camera camera(glm::vec3(0.0f, 2.0f, 3.0f));
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int constexpr WIDTH = 800;
int constexpr HEIGHT = 600;

int main() {
    // create and configure window
    GLFWwindow* window = createAndConfigureWindow();
    if (window == nullptr) {
        return -1;
    }

    // Render instantiations
    // ---------------------
    TextRenderer textRenderer(WIDTH, HEIGHT);
    EntityRenderer entityRenderer(&camera);
    TerrainRenderer terrainRenderer(&camera, &entityRenderer);
    Terrain terrain(
        "/Users/vladino/CLionProjects/mygame/resources/images/heightmaps/heightmap.png",
        "/Users/vladino/CLionProjects/mygame/resources/images/blendMap.png"
    );
    Fps fps;

    // Player related code
    // -------------------
    Model wolf("/Users/vladino/CLionProjects/mygame/resources/objects/animals/wolf2/Wolf_One_obj.obj");
    Player player(
        camera,
        terrain,
        wolf,
        glm::vec3(20.0f, 0.0f, -10.0f)
    );
    // TODO: Temp until we rotate player based on terrain angles
    player.SetRotateX(1.0f);
    RandomPlayerMover playerMover(player);

    glEnable(GL_DEPTH_TEST);

    // enabling this will draw only lines
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_callback);

    while (!glfwWindowShouldClose(window)) {
        // delta calculation
        calculateDelta();

        // fps calculation
        fps.tick();

        // input
        // -----
        processInput(window);
        playerMover.move(deltaTime);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        terrainRenderer.render(terrain);
        entityRenderer.render(&player);

        // textRenderer.RenderBlackText("gameformykids", WIDTH - 170, HEIGHT - 30, 0.45f);
        // TODO: Muted until I find more efficient way to render text
        // textRenderer.RenderBlackText(fps.getFpsAsString(), 25.0f, 25.0f, 0.25f);
        // textRenderer.RenderBlackText("camera x:" + std::to_string(camera.Position.x) + " y:" + std::to_string(camera.Position.y) + " z:" + std::to_string(camera.Position.z), 25.0f, 50.0f, 0.25f);
        // std::cout << "camera x:" << camera.Position.x << " y:" << camera.Position.y << " z:" << camera.Position.z << std::endl;

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


GLFWwindow* createAndConfigureWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "gameformykids", NULL, NULL);
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

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void calculateDelta() {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}
