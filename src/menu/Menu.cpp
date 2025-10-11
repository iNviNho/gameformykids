#include "Menu.h"

void Menu::KeyboardEscapePressed() {
    // we ignore if escape is pressed and we just started game
    if (gameState.isInMenuAndGameDidNotStart()) {
        return;
    }
    gameState.changeToInMenuGameAlreadyStarted();
    mainDiv.RecalculateElementsPositions();
}

void Menu::MouseButtonLeftClicked(double xPosition, double yPosition) {
    // this is just passthrough
    // we will delegate processing to main div
    mainDiv.MouseButtonLeftClicked(xPosition, yPosition);
}

void Menu::MouseHovered(double xPosition, double yPosition) {
    // this is just passthrough
    // we will delegate processing to main div
    mainDiv.MouseHovered(xPosition, yPosition);
}

void Menu::Render() {
    uiRenderer.Render(mainDiv);
}