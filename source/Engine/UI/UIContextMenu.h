//
// Created by Elliot Fiske on 11/20/18.
//
// UIContextMenu.h
//
// Attaches to an element and reveals a list of options when you click on it.
//

#pragma once

#include "UIStackView.h"

namespace CubeWorld
{

namespace Engine
{

class UIContextMenu : public UIElement
{
public:
   /**
    * Represents an option the user can choose from the UIContextMenu. A TextButton
    *   will be created with the speicifed name and callback.
    */
   struct Choice {
      std::string name;                   ///< Text displayed for this choice
      std::function<void(void)> callback; ///< Called when this choice is selected from the menu
   };

   UIContextMenu(UIRoot* root, UIElement* parent, const std::string &name, const std::list<Choice> &choices);

private:
   UIStackView* mOptionList;       ///< Organizes the options in a vertical list
   UIElement*   mBoundingElement;  ///< The context menu must appear within the bounds of this element.

   std::vector<Choice> mChoices;

};

}  // Engine

}  // Cubeworld

