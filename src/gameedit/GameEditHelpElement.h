#ifndef MYGAME_GAMEEDITELEMENT_H
#define MYGAME_GAMEEDITELEMENT_H

#include "../ui/Element.h"

class GameEditHelpElement {

public:
    GameEditHelpElement(Screen& screen): element(Element{screen}) {
        
        element.SetVerticalPosition(Element::VerticalPositions::MIDDLE);
        element.SetHorizontalPosition(Element::HorizontalPositions::RIGHT);
        element.GetSubElements().reserve(2);

        Element welcomeText = Element{screen};
        welcomeText.SetText("You are in GAME EDIT MODE.");
        welcomeText.SetTextScale(0.5f);
        welcomeText.SetIsBlock(true);
        welcomeText.SetMarginBottom(20.0f);
        element.AddElement(std::move(welcomeText));

        Element objectsText = Element{screen};
        objectsText.SetText("Select an object you wish to place by pressing Q.\n"
                "Rotate an object in any axis by pressing R, T or Y.\n"
                "To counter-rotate in any axis press and hold ctrl and then pres R, T or Y.\n"
                "To place an object press left mouse button.\n"
                "To remove an object, hold X and press left mouse button.\n"
                "To scale up an object, press E.\n"
                "To scale down an object, press ctrl E.\n");
        objectsText.SetTextScale(0.4f);
        objectsText.SetMaxWidth(400.0f);
        objectsText.SetIsBlock(true);
        element.AddElement(std::move(objectsText));

        element.RecalculateElementsPositions();
    }

    Element& GetElement() { return element;} 
private:
    Element element;


};



#endif
