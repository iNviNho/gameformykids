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
        objectsText.SetText(
            "OBJECT PLACEMENT\n"
            "Q              Cycle through objects to place\n"
            "Left Click     Place the selected object\n"
            "X + Left Click Remove an object\n"
            "\n"
            "OBJECT TRANSFORM\n"
            "R / T / Y      Rotate on each axis\n"
            "Ctrl + R/T/Y   Rotate in the opposite direction\n"
            "E              Scale up\n"
            "Ctrl + E       Scale down\n"
            "\n"
            "TERRAIN EDITING\n"
            "F              Enter terrain editing mode\n"
            "Left Click     Raise terrain\n"
            "Ctrl + Left Click Lower terrain\n"
            "G              Reset terrain height\n"
            "O              Increase brush radius\n"
            "Ctrl + O       Decrease brush radius\n"
        );       
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
