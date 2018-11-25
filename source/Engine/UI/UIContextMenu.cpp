//
// Created by Elliot Fiske on 11/20/18.
//
// UIContextMenu.cpp
//
// Attaches to an element and reveals a list of options when you click on it.
//

#include "UIContextMenu.h"

#include <Shared/Helpers/Asset.h>
#include <Shared/UI/TextButton.h>

#include "UISerializationHelper.h"



namespace CubeWorld
{

namespace Engine
{

using UI::TextButton;

/**
 * Instantiate a new UIContextMenu.
 */
UIContextMenu::UIContextMenu(UIRoot* root, UIElement* parent, const std::string &name, const std::list<Choice> &choices)
      : UIElement(root, parent, name)
{
   Engine::UISerializationHelper serializer;
   Engine::ElementsByName elementMap = serializer.CreateUIFromJSONFile(Paths::Normalize(Asset::UIElement("context_menu.json")), mpRoot, this);

   UIElement* mainBackground = elementMap["ContextMenuBackgroundBorder"];
   mainBackground->ConstrainEqualBounds(this);

   mOptionList = (UIStackView*) elementMap["ContextMenuItemList"];

   // Instantiate choices
   for (Choice c : choices) {
      TextButton::Options buttonOptions;
      buttonOptions.text = c.name;
      buttonOptions.onClick = c.callback;
      TextButton* load = mOptionList->Add<TextButton>(buttonOptions);

      load->ConstrainLeftAlignedTo(mOptionList);
      load->ConstrainRightAlignedTo(mOptionList);
      load->ConstrainHeight(32);
   }
}


}  // Engine

}  // Cubeworld
