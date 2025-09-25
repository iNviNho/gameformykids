#ifndef MYGAME_UIRENDERER_H
#define MYGAME_UIRENDERER_H
#include "Element.h"
#include "../models/StaticShapeRenderer.h"
#include "../text/TextRenderer.h"

using path = std::filesystem::path;

class UiRenderer {
public:
    UiRenderer(
        const TextRenderer& textRenderer,
        const StaticShapeRenderer& staticShapeRenderer
    ):
    textRenderer(textRenderer),
    staticShapeRenderer(staticShapeRenderer)
    {}
    void Render(Element& element);
private:
    TextRenderer textRenderer;
    StaticShapeRenderer staticShapeRenderer;
};


#endif //MYGAME_UIRENDERER_H