#ifndef MYGAME_MENU_H
#define MYGAME_MENU_H
#include "data_dir.h"
#include "../models/StaticShape.h"
#include "../textures/TextureLoader.h"
#include "../ui/UiRenderer.h"
#include "../utils/GameState.h"
#include "GLFW/glfw3.h"

using path = std::filesystem::path;

class Menu {
public:
    explicit Menu(
        GameState& gameState,
        UiRenderer& uiRenderer,
        GLFWwindow* window,
        Screen& screen
    ):
    gameState(gameState),
    uiRenderer(uiRenderer),
    window(window),
    screen(screen),
    mainDiv(Element{screen})
    {
        /**
         * CONSTRUCT "MAIN" DIV
        */
        mainDiv.SetHorizontalPosition(Element::HorizontalPositions::CENTER);
        StaticShape staticShape{
            data_dir() /= path("resources/images/backgroundImage/wolf2.png"),
        };
        mainDiv.SetBackgroundImage(std::move(staticShape));
        // 3 elements: play, settings and quit
        mainDiv.GetSubElements().reserve(3);

        /**
         * CONSTRUCT "PLAY" BUTTON
        */
        Element playDiv = Element{screen};
        playDiv.SetText("PLAY");
        playDiv.SetMarginBottom(15.0f);
        playDiv.SetOnClick([&gameState](Element&) {
            Log::logInfo("[MENU]: Play clicked");
            gameState.changeToInGame();
        });
        playDiv.SetOnMouseEnter([](Element& e) {
            e.SetTextScale(1.05f);
        });
        playDiv.SetOnMouseLeave([](Element& e) {
            e.SetTextScale(1.0f);
        });
        mainDiv.AddElement(std::move(playDiv));

        /**
         * CONSTRUCT "SETTINGS" BUTTON
        */
        Element settingsDiv = Element{screen};
        settingsDiv.SetText("SETTINGS");
        settingsDiv.SetMarginBottom(15.0f);
        settingsDiv.SetMarginLeft(20.0f);
        settingsDiv.SetOnClick([](Element&) {
            Log::logInfo("[MENU]: Settings clicked");
            // todo: we are going to open settings menu
        });
        settingsDiv.SetOnMouseEnter([](Element& e) {
            e.SetTextScale(1.05f);
        });
        settingsDiv.SetOnMouseLeave([](Element& e) {
            e.SetTextScale(1.0f);
        });
        mainDiv.AddElement(std::move(settingsDiv));

        /**
         * CONSTRUCT "QUIT" BUTTON
        */
        Element quitDiv = Element{screen};
        quitDiv.SetText("QUIT");
        quitDiv.SetMarginBottom(15.0f);
        quitDiv.SetMarginLeft(20.0f);
        quitDiv.SetOnClick([window](Element&) {
            Log::logInfo("[MENU]: Quit clicked");
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        });
        quitDiv.SetOnMouseEnter([](Element& e) {
            e.SetTextScale(1.05f);
        });
        quitDiv.SetOnMouseLeave([](Element& e) {
            e.SetTextScale(1.0f);
        });
        mainDiv.AddElement(std::move(quitDiv));

        /**
         * Calculate all positions of div and children divs
        */
        mainDiv.RecalculateElementsPositions();
    }

    void MouseButtonLeftClicked(double xPosition, double yPosition);
    void MouseHovered(double xPosition, double yPosition);
    void KeyboardEscapePressed();
    void Render();

private:
    GameState& gameState;
    UiRenderer& uiRenderer;
    GLFWwindow* window;
    Screen& screen;
    Element mainDiv;
};


#endif //MYGAME_MENU_H