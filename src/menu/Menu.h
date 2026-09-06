#ifndef MYGAME_MENU_H
#define MYGAME_MENU_H
#include "data_dir.h"
#include "../models/StaticShape.h"
#include "../ui/UiRenderer.h"
#include "../utils/GameState.h"
#include "GLFW/glfw3.h"
#include "../objects/movers/PathPlayerMover.h"

using path = std::filesystem::path;

class Menu {
public:
    explicit Menu(
        GameState& gameState,
        UiRenderer& uiRenderer,
        GLFWwindow* window,
        Screen& screen,
        PathPlayerMover& pathPlayerMover
    ):
    gameState(gameState),
    uiRenderer(uiRenderer),
    window(window),
    screen(screen),
    mainDiv(Element{screen}),
    settingsDiv(Element{screen}),
    pathPlayerMover(pathPlayerMover)
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
        playDiv.SetText("Play");
        playDiv.SetMarginBottom(15.0f);
        playDiv.SetOnClick([&gameState](Element&) {
            Log::logInfo("[MENU]: Play clicked");
            gameState.changeToStartGame();
        });
        playDiv.SetOnMouseEnter([](Element& e) {
            e.SetTextScale(1.05f);
        });
        playDiv.SetOnMouseLeave([](Element& e) {
            e.SetTextScale(1.0f);
        });
        playDiv.SetVisibilityCondition([&gameState]() {
           return gameState.isInMenuAndGameDidNotStart();
        });
        mainDiv.AddElement(std::move(playDiv));

        /**
         * CONSTRUCT "RESUME" BUTTON
        */
        Element resumeDiv = Element{screen};
        resumeDiv.SetText("Resume");
        resumeDiv.SetMarginBottom(15.0f);
        resumeDiv.SetOnClick([&gameState](Element&) {
            Log::logInfo("[MENU]: Resume clicked");
            gameState.changeToStartGame();
        });
        resumeDiv.SetOnMouseEnter([](Element& e) {
            e.SetTextScale(1.05f);
        });
        resumeDiv.SetOnMouseLeave([](Element& e) {
            e.SetTextScale(1.0f);
        });
        resumeDiv.SetVisibilityCondition([&gameState] {
           return gameState.isInMenuAndGameAlreadyStarted();
        });
        mainDiv.AddElement(std::move(resumeDiv));
        
        /**
         * CONSTRUCT "RESTART" BUTTON
        */
        Element restartDiv = Element{screen};
        restartDiv.SetText("Restart");
        restartDiv.SetMarginBottom(15.0f);
        restartDiv.SetMarginLeft(20.0f);
        restartDiv.SetOnClick([&pathPlayerMover, &gameState](Element&) {
            Log::logInfo("[MENU]: Restart clicked");
            gameState.changeToRestartGame();
            pathPlayerMover.Reset();
        });
        restartDiv.SetOnMouseEnter([](Element& e) {
            e.SetTextScale(1.05f);
        });
        restartDiv.SetOnMouseLeave([](Element& e) {
            e.SetTextScale(1.0f);
        });
        restartDiv.SetVisibilityCondition([&gameState] {
           return gameState.isInMenuAndGameAlreadyStarted();
        });
        mainDiv.AddElement(std::move(restartDiv));

        

        /**
         * CONSTRUCT "SETTINGS" BUTTON
        */
        Element settingsText = Element{screen};
        settingsText.SetText("Settings");
        settingsText.SetMarginBottom(15.0f);
        settingsText.SetMarginLeft(20.0f);
        settingsText.SetOnClick([&gameState, this](Element& settingsText) {
            Log::logInfo("[MENU]: Settings clicked");
            gameState.toggleSettingsState();
            settingsDiv.RecalculateElementsPositions();
        });
        settingsText.SetOnMouseEnter([](Element& e) {
            e.SetTextScale(1.05f);
        });
        settingsText.SetOnMouseLeave([](Element& e) {
            e.SetTextScale(1.0f);
        });

        settingsText.RecalculateElementsPositions();

        mainDiv.AddElement(std::move(settingsText));

        /**
         * CONSTRUCT "QUIT" BUTTON
        */
        Element quitDiv = Element{screen};
        quitDiv.SetText("Quit");
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


        /**
         * SETTINGS DIV
         */
        /** 
         *
         */
        settingsDiv.SetHorizontalPosition(Element::HorizontalPositions::CENTER);
        settingsDiv.SetVerticalPosition(Element::VerticalPositions::MIDDLE);
        
        Element gameEditModeToggle = Element{screen};
        gameEditModeToggle.SetText("Enable game edit mode");
        gameEditModeToggle.SetTextScale(0.8f);
        gameEditModeToggle.SetOnClick([&gameState](Element& e) {
            Log::logInfo("[MENU][SETTINGS]: Toggle game edit mode");
            gameState.toggleGameEditMode();
            e.SetText(gameState.isGameEditModeEnabled() ? "Disable game edit mode" : "Enable game edit mode");
        });
        gameEditModeToggle.SetOnMouseEnter([](Element& e) {
            e.SetTextScale(0.85f);
        });
        gameEditModeToggle.SetOnMouseLeave([](Element& e) {
            e.SetTextScale(0.8f);
        });
        gameEditModeToggle.SetVisibilityCondition([&gameState] {
           return gameState.isSettingOpen();
        });
        settingsDiv.AddElement(std::move(gameEditModeToggle));

        settingsDiv.RecalculateElementsPositions();
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
    Element settingsDiv;
    PathPlayerMover& pathPlayerMover;
};


#endif //MYGAME_MENU_H
