//
// Created by Elliot Fiske on 11/20/18.
//
// UIContextMenu.h
//
// Attaches to an element and reveals a list of options when you click on it.
//
// Also defines the class that manages UIContextMenus.
//

#pragma once

#include "UIStackView.h"

namespace CubeWorld
{

namespace Engine
{

class UIContextMenu : public UIElementDep
{
public:
   /**
    * Represents an option the user can choose from the UIContextMenu. A TextButton
    *   will be created with the specified name and callback.
    */
   struct Choice {
      std::string name;                   ///< Text displayed for this choice
      std::function<void(void)> callback; ///< Called when this choice is selected from the menu
   };

   typedef std::list<Choice> Choices;

   UIContextMenu(UIRootDep* root, UIElementDep* parent, const std::string &name, const Choices &choices);

private:
   UIStackView* mOptionList;       ///< Organizes the options in a vertical list
   UIElementDep*   mBoundingElement;  ///< The context menu must appear within the bounds of this element.

   std::vector<Choice> mChoices;
};

//
// This class lives in the UIRootDep, positions the UIContextMenu and manages
//   its lifecycle.
//
class UIContextMenuParent : public UIElementDep
{
public:
   UIContextMenuParent(UIRootDep* root, UIElementDep* parent, const std::string &name = "");

   void CreateNewUIContextMenu(double x, double y, UIContextMenu::Choices choices);

   Action MouseClick(const MouseClickEvent &event) override;

private:
   UIContextMenu* mCurrentMenu;
   bool mbIsShowingContextMenu = false;
};

}  // Engine

}  // Cubeworld
