#include "Element.h"

#include "../utils/Log.h"
#include <sstream>

void Element::calculateSubElementTextsToRender() {
    subElementTextsToRender.clear();

    // No max width -> still respect explicit newlines.
    if (maxWidth == 0.0f) {
        std::stringstream stream(text);
        std::string line;
        int lineIndex = 0;
        float lineHeight = textProportion.GetHeight();

        while (std::getline(stream, line)) {
            subElementTextsToRender.emplace_back(
                line,
                glm::vec2{0.0f, lineIndex * lineHeight}
            );
            ++lineIndex;
        }

        return;
    }

    float lineHeight = textProportion.GetHeight();
    int lineIndex = 0;

    std::stringstream stream(text);
    std::string paragraph;

    // Process each explicitly separated line.
    while (std::getline(stream, paragraph)) {

        std::istringstream wordStream(paragraph);

        std::string word;
        std::string currentLine;

        while (wordStream >> word) {
            std::string candidate = currentLine.empty()
                ? word
                : currentLine + " " + word;

            TextProportion candidateProportion{
                candidate,
                GetTextScale()
            };

            if (!currentLine.empty() &&
                candidateProportion.GetWidth() > maxWidth) {

                subElementTextsToRender.emplace_back(
                    currentLine,
                    glm::vec2{
                        0.0f,
                        lineIndex * lineHeight
                    }
                );

                currentLine = word;
                ++lineIndex;
            } else {
                currentLine = candidate;
            }
        }

        // Add remaining text from this explicit line.
        if (!currentLine.empty()) {
            subElementTextsToRender.emplace_back(
                currentLine,
                glm::vec2{
                    0.0f,
                    lineIndex * lineHeight
                }
            );
        }

        // Explicit \n means the next text starts on a new line.
        ++lineIndex;
    }

    // Handle text ending with '\n'.
    if (!text.empty() && text.back() == '\n') {
        subElementTextsToRender.emplace_back(
            "",
            glm::vec2{
                0.0f,
                lineIndex * lineHeight
            }
        );
    }
}

void Element::MouseButtonLeftClicked(double xPosition, double yPosition) {
    Log::logInfo("Left mouse button clicked at: " + std::to_string(xPosition) + ", " + std::to_string(yPosition));
    handleElementClick(*this, xPosition, yPosition);
}

void Element::handleElementClick(Element& elementGiven, double xPosition, double yPosition) {
    float yPositionInverted = abs(yPosition - screen.GetHeight());
    for (auto& element : elementGiven.GetSubElements()) {
        // we ignore invisible elements
        if (!element.GetVisibilityCondition()()) {
            continue;
        }

        if (xPosition >= element.GetPosition().x && xPosition <= element.GetPosition().x + element.GetTextProportion().GetWidth() &&
            yPositionInverted >= element.GetPosition().y && yPositionInverted <= element.GetPosition().y + element.GetTextProportion().GetHeight()) {
            element.GetOnClick()(element);
            return;
            }

        // if sub element has sub elements, we need to delegate the click to them
        if (element.HasSubElements()) {
            handleElementClick(element, xPosition, yPosition);
        }
    }
}

void Element::MouseHovered(double xPosition, double yPosition) {
    float yPositionInverted = abs(yPosition - screen.GetHeight());
    for (auto& element : GetSubElements()) {
        if (xPosition >= element.GetPosition().x && xPosition <= element.GetPosition().x + element.GetTextProportion().GetWidth() &&
            yPositionInverted >= element.GetPosition().y && yPositionInverted <= element.GetPosition().y + element.GetTextProportion().GetHeight()) {
            // we ignore invisible elements
            if (!element.GetVisibilityCondition()()) {
                continue;
            }

            element.GetOnMouseEnter()(element);
        } else {
            element.GetOnMouseLeave()(element);
        }
    }
}

void Element::RecalculateElementsPositions() {

    float startX;

    float totalWidth = 0.0f;
    float previousMarginRight = 0.0f;

    // Calculate total rendered width
    for (auto& element : GetSubElements()) {
        if (!element.GetVisibilityCondition()()) {
            continue;
        }
        
        if (element.isBlock) {
            auto width = (element.maxWidth > 0.0f ? element.maxWidth : element.GetTextProportion().GetWidth());
            totalWidth = width > totalWidth ? width : totalWidth;
        } else {
            totalWidth += previousMarginRight
                        + element.GetMarginLeft()
                        + (element.maxWidth > 0.0f ? element.maxWidth : element.GetTextProportion().GetWidth());
        }
        previousMarginRight = element.GetMarginRight();
    }

    if (this->horizontalPosition == CENTER) {
        startX = screen.GetWidth() / 2.0f - totalWidth / 2.0f;
    }
    else if (this->horizontalPosition == RIGHT) {
        startX = screen.GetWidth() - 400.0f;
    }
    else {
        startX = 0.0f;
    }

    float renderedWidth = 0.0f;
    float renderedHeight = 0.0f;
    previousMarginRight = 0.0f;

    for (auto& element : GetSubElements()) {
        if (!element.GetVisibilityCondition()()) {
            continue;
        }

        float yPosition = element.GetMarginBottom();

        if (this->verticalPosition == MIDDLE) {
            yPosition += screen.GetHeight() / 2.0f;
        }

        element.SetPosition(glm::vec2{
            startX
                + renderedWidth
                + previousMarginRight
                + element.GetMarginLeft(),

            yPosition + renderedHeight
        });

        // we set rendered width only if it is NOT block so the next element goes on next line
        if (!element.IsBlock()) {
            renderedWidth += previousMarginRight
                           + element.GetMarginLeft()
                           + (element.maxWidth > 0.0f ? element.maxWidth : element.GetTextProportion().GetWidth());
        } else {
            renderedHeight -= element.GetMarginBottom()
                            // TODO: This will not work for block elements that have maxWidth
                            // Then we have to calculate GetHeight * actual amount of lines
                            + element.GetTextProportion().GetHeight();
        }
        previousMarginRight = element.GetMarginRight();
    }
}
