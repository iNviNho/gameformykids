#ifndef MYGAME_ELEMENT_H
#define MYGAME_ELEMENT_H
#include <string>

#include "Screen.h"
#include "../models/StaticShape.h"
#include "../text/TextProportion.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

/**
 * The main point of this class is to imitate the functionality of <div> element in HTML5 syntax
 *
 * It should allow (in a simplistic way) to create a UI layout hierarchically
 *
 * Each element can contain sub-elements, which will be positioned relative to their parent element
 *
 */
class Element {
public:
    // todo: understand how these work so we make object move-only (no copy)
    // Element(const Element&) = delete;
    // Element& operator=(const Element&) = delete;
    // Element(Element&&) = default;
    // Element& operator=(Element&&) = default;
    enum HorizontalPositions {
        LEFT,
        CENTER,
        RIGHT
    };
    enum VerticalPositions {
        TOP,
        MIDDLE,
        BOTTOM
    };

    explicit Element(
        Screen& screen
    ):
    textProportion(TextProportion{"", 1.0f}),
    screen(screen)
    {}

    const std::string& GetText() const { return text; }
    const glm::vec3& GetTextColor() const { return textColor; }
    std::vector<Element>& GetSubElements() { return subElements; }
    const glm::vec2& GetPosition() const { return position; }
    const TextProportion& GetTextProportion() const { return textProportion; }
    float GetMarginBottom() const { return marginBottom; }
    float GetMarginLeft() const { return marginLeft; }
    float GetMarginRight() const { return marginRight; }
    std::function<void(Element&)>& GetOnClick() { return onClick; }
    const std::function<void(Element&)>& GetOnMouseEnter() const { return onMouseEnter; }
    const std::function<void(Element&)>& GetOnMouseLeave() const { return onMouseLeave; }
    float GetTextScale() const { return textScale; }
    const std::optional<StaticShape>& GetBackgroundImage() const { return backgroundImage; }

    void SetText(const std::string& newText) {
        this->text = newText;
        this->textProportion = TextProportion{newText, GetTextScale()};
    }
    void SetMarginBottom(const float newMarginBottom) { this->marginBottom = newMarginBottom; }
    void SetMarginLeft(const float newMarginLeft) { this->marginLeft = newMarginLeft; }
    void SetMarginRight(const float newMarginRight) { this->marginRight = newMarginRight; }
    void SetHorizontalPosition(const HorizontalPositions& newHorizontalPosition) {this->horizontalPosition = newHorizontalPosition;}
    void SetOnClick(const std::function<void(Element&)>& onClickNew) { this->onClick = onClickNew; }
    void SetOnMouseEnter(const std::function<void(Element&)>& onMouseEnterNew) { this->onMouseEnter = onMouseEnterNew; }
    void SetOnMouseLeave(const std::function<void(Element&)>& onMouseLeaveNew) { this->onMouseLeave = onMouseLeaveNew; }
    void SetTextScale(float newTextScale) {
        if (newTextScale == textScale) {
            return;
        }
        this->textScale = newTextScale;
        this->textProportion = TextProportion{text, GetTextScale()};
    }
    void AddElement(Element&& element) {
        subElements.emplace_back(std::move(element));
    }
    bool HasSubElements() const { return !subElements.empty(); }

    void MouseButtonLeftClicked(double xPosition, double yPosition);
    void MouseHovered(double xPosition, double yPosition);

    void RecalculateElementsPositions();

    void SetBackgroundImage(StaticShape&& staticShape) {
        this->backgroundImage = std::move(staticShape);
    }

private:
    Screen& screen;

    std::optional<StaticShape> backgroundImage;

    std::string text{};
    TextProportion textProportion;
    glm::vec3 textColor{1.0f, 1.0f, 1.0f};
    float textScale{1.0f};

    glm::vec2 position{0.0f, 0.0f};
    HorizontalPositions horizontalPosition{CENTER};
    VerticalPositions verticalPosition{TOP};
    float marginBottom{};
    float marginLeft{};
    float marginRight{};

    std::vector<Element> subElements{};

    std::function<void(Element&)> onClick{[](Element&) {}};
    std::function<void(Element&)> onMouseEnter{[](Element&) {}};
    std::function<void(Element&)> onMouseLeave{[](Element&) {}};

    void SetPosition(const glm::vec2& newPosition) { this->position = newPosition; }
};


#endif //MYGAME_ELEMENT_H