#include "Element.h"

#include "../utils/Log.h"

void Element::MouseButtonLeftClicked(double xPosition, double yPosition) {
    Log::logInfo("Left mouse button clicked at: " + std::to_string(xPosition) + ", " + std::to_string(yPosition));
    float yPositionInverted = abs(yPosition - screen.GetHeight());
    for (auto& element : GetSubElements()) {
        // we ignore invisible elements
        if (!element.GetVisibilityCondition()()) {
            continue;
        }

        if (xPosition >= element.GetPosition().x && xPosition <= element.GetPosition().x + element.GetTextProportion().GetWidth() &&
            yPositionInverted >= element.GetPosition().y && yPositionInverted <= element.GetPosition().y + element.GetTextProportion().GetHeight()) {
            element.GetOnClick()(element);
            return;
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

    // something on the element changed
    // that means we need to recalculate x and y position of this
    // and all other elements

    // if horizontal position is CENTER,
    // we simply calculate width of all elements and put them to the middle
    if (this->horizontalPosition == CENTER) {
        float mid = screen.GetWidth() / 2.0f;
        float midOffset = 0.0f;
        float nextElementMarginRight = 0.0f;
        for (auto& element: GetSubElements()) {
            // not for invisible elements
            if (!element.GetVisibilityCondition()()) {
                continue;
            }

            midOffset += element.GetTextProportion().GetWidth() + nextElementMarginRight + element.GetMarginLeft();
            nextElementMarginRight = element.GetMarginRight();
        }
        nextElementMarginRight = 0.0f;

        float renderedWidth = 0.0f;

        // When rendering text, x:0 means left and y:0 means bottom
        // y:0 also means text is built from BOTTOM to TOP so it will be fully rendered
        for (auto& element : GetSubElements()) {
            // not for invisible elements
            if (!element.GetVisibilityCondition()()) {
                continue;
            }

            element.SetPosition(glm::vec2{
                mid + element.GetMarginLeft() + nextElementMarginRight + renderedWidth - midOffset / 2,
                // When rendering text, x:0 means left and y:0 means bottom
                // y:0 also means text is built from BOTTOM to TOP so it will be fully rendered
               element.GetMarginBottom(),
            });
            nextElementMarginRight = element.GetMarginRight();
            renderedWidth += element.GetTextProportion().GetWidth() + nextElementMarginRight + element.GetMarginLeft();
        }
    } else {
        Log::logError("Only HorizontalPosition::CENTER implemented.");
    }
}
