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
#include "src/models/ModelRenderer.h"
#include "src/terrain/Terrain.h"
#include "src/terrain/TerrainRenderer.h"
#include "src/text/TextRenderer.h"

int add(int x, int y);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
GLFWwindow* createAndConfigureWindow();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int loadTexture(char const* value);
void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color);
void processInput(GLFWwindow *window);

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = 400, lastY = 300;
bool firstMouse = false;
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int constexpr  WIDTH = 800;
int constexpr  HEIGHT = 600;

int main() {
    // create and configure window
    GLFWwindow* window = createAndConfigureWindow();
    if (window == nullptr) {
        return -1;
    }
    // create shader program
    Shader myShader(
    "/Users/vladino/CLionProjects/mygame/src/shaders/files/shader.vs",
    "/Users/vladino/CLionProjects/mygame/src/shaders/files/shader.fs"
    );

    Shader lightShader(
    "/Users/vladino/CLionProjects/mygame/src/shaders/files/lightShader.vs",
    "/Users/vladino/CLionProjects/mygame/src/shaders/files/lightShader.fs"
    );

    TextRenderer textRenderer(WIDTH, HEIGHT);
    TerrainRenderer terrainRenderer(&camera);
    ModelRenderer modelRenderer(&camera);
    Terrain terrain(0, 0, "/Users/vladino/CLionProjects/mygame/resources/images/heightmaps/heightmap.png");
    Fps fps;

    glEnable(GL_DEPTH_TEST);

    // enabling this will draw only lines
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Model backpack(
        "/Users/vladino/CLionProjects/mygame/resources/objects/backpack/backpack.obj"
    );

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    // generate VAO & VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // bind vertex
    glBindVertexArray(VAO);
    // bind buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // copy data to buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // unsigned int texture = TextureLoader::loadTexture("/Users/vladino/CLionProjects/mygame/container.jpg");

    glm::mat4 projection;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // fps calculation
        fps.tick();

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // // glBindVertexArray(terrainVAO2);
        // // glDrawArrays(GL_TRIANGLES, 0, terrain2.GetCountOfVertices());
        //
        // myShader.use();
        //
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, texture);
        //
        // // light properties
        // myShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        // myShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        // myShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        // myShader.setVec3("light.position", lightPos);
        // myShader.setVec3("viewPos", camera.Position);
        //
        // // material properties
        // myShader.setInt("material.diffuse", 0); // texture unit
        // myShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        // myShader.setFloat("material.shininess", 64.0f);
        //
        //
        // // world transformation
        // // The next step is to transform the local coordinates to world-space coordinates which are
        // // coordinates in respect of the world origin.
        // // We can move object, rotate it, scale it, etc.
        // glm::mat4 model = glm::mat4(1.0f);
        // // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
        // myShader.setMat4("model", model);
        //
        // // view transformation
        // // coordinates are modified in such a way that
        // // each coordinate is seen from cameras points of view
        // glm::mat4 view = camera.GetViewMatrix();
        // myShader.setMat4("view", view);
        //
        // // projection transformation
        // projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        // myShader.setMat4("projection", projection);
        //
        // glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        //
        // // lightning shader
        // lightShader.use();
        //
        // lightShader.setMat4("projection", projection);
        // lightShader.setMat4("view", view);
        //
        // model = glm::translate(model, lightPos);
        // model = glm::scale(model, glm::vec3(0.2f));
        //
        // lightShader.setMat4("model", model);
        //
        // glBindVertexArray(VAO);
        // // glDrawArrays(GL_TRIANGLES, 0, 36);

        terrainRenderer.render(terrain);

        modelRenderer.render(backpack);

        textRenderer.RenderBlackText("gameformykids", WIDTH - 170, HEIGHT - 30, 0.45f);
        textRenderer.RenderBlackText("fps:" + std::to_string(fps.getFps()), 25.0f, 25.0f, 0.25f);
        textRenderer.RenderBlackText("camera x:" + std::to_string(camera.Position.x) + " y:" + std::to_string(camera.Position.y) + " z:" + std::to_string(camera.Position.z), 25.0f, 50.0f, 0.25f);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    return 0;
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
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
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

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}