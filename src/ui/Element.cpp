#include "Element.h"

#include "../utils/Log.h"

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

    if (this->horizontalPosition == CENTER) {
        const float midX = screen.GetWidth() / 2.0f;

        float totalWidth = 0.0f;
        float previousMarginRight = 0.0f;

        // Calculate total rendered width of all visible elements
        for (auto& element : GetSubElements()) {
            if (!element.GetVisibilityCondition()()) {
                continue;
            }

            totalWidth += previousMarginRight
                        + element.GetMarginLeft()
                        + element.GetTextProportion().GetWidth();

            previousMarginRight = element.GetMarginRight();
        }

        float renderedWidth = 0.0f;
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
                midX
                    - totalWidth / 2.0f
                    + renderedWidth
                    + previousMarginRight
                    + element.GetMarginLeft(),

                yPosition
            });

            renderedWidth += previousMarginRight
                           + element.GetMarginLeft()
                           + element.GetTextProportion().GetWidth();

            previousMarginRight = element.GetMarginRight();
        }

    } else {
        Log::logError("Only HorizontalPosition::CENTER implemented.");
    }
}
