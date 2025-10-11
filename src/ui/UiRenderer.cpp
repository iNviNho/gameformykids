#include "UiRenderer.h"

void UiRenderer::Render(Element& element) {

    if (element.GetBackgroundImage().has_value()) {
        // TODO: Missing ability to define x,y coord and with, height
        staticShapeRenderer.Render(element.GetBackgroundImage().value());
    }

    if (element.GetVisibilityCondition()()) {
        textRenderer.RenderText(
            element.GetText(),
            element.GetPosition().x,
            element.GetPosition().y,
            element.GetTextScale(),
            element.GetTextColor()
        );
    }

    // we need to do recursive rendering
    if (element.HasSubElements()) {
        for (auto& subElement: element.GetSubElements()) {
            Render(subElement);
        }
    }

}
