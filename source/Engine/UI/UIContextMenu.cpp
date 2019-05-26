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
#include <Shared/UI/RectFilled.h>

#include "UISerializationHelper.h"

#include <RGBBinding/Observable.h>

namespace CubeWorld
{

namespace Engine
{

using UI::TextButton;
using UI::RectFilled;

/**
 * Instantiate a new UIContextMenu.
 */
UIContextMenu::UIContextMenu(UIRoot* root, UIElement* parent, const std::string &name, const std::list<Choice> &choices)
      : UIElement(root, parent, name)
{
   std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();
   ObservableInternal<int> testObservable;
   
   testObservable.OnChanged()
//   .Map<bool>([](int test) -> bool {
//      return test % 2;
//   })
   .Subscribe([](int test) {
      std::cout << "OH yeAH BABY" << test;
   }, myBag);
   
   testObservable.SendMessage(3);
   testObservable.SendMessage(4);
   testObservable.SendMessage(5);
   testObservable.SendMessage(-1);
   
   Engine::UISerializationHelper serializer;
   Maybe<Engine::ElementsByName> maybeElementMap = serializer.CreateUIFromJSONFile(Paths::Normalize(Asset::UIElement("context_menu.json")), mpRoot, this);

   assert(maybeElementMap && "Unable to create UI for context menu.");

   Engine::ElementsByName elementMap = *maybeElementMap;

   UIElement* mainBackground = elementMap["ContextMenuBackgroundBorder"];
   mainBackground->ConstrainEqualBounds(this);

   mOptionList = (UIStackView*) elementMap["ContextMenuItemList"];
   mOptionList->SetAlignItemsBy(UIStackView::Left);

   // Instantiate choices
   for (Choice c : choices) {
      TextButton::Options buttonOptions;
      buttonOptions.text = c.name;
      buttonOptions.onClick = c.callback;
      TextButton* load = mOptionList->Add<TextButton>(buttonOptions);

      load->ConstrainWidthToContent();
      load->ConstrainHeightToContent();
      
      UIConstraint::Options opts;
      opts.relationship = UIConstraint::GreaterThanOrEqual;
      mOptionList->ConstrainWidthTo(load, 0, 1.0, opts);
   }
}

/**
 * Creates a new UIContextMenu within this element's bounds.
 *
 * @param x, y      Absolute position within the UIRoot where the context menu will appear
 * @param choices   List of choices for the menu
 */
void UIContextMenuParent::CreateNewUIContextMenu(double x, double y, UIContextMenu::Choices choices)
{
   mCurrentMenu = Add<UIContextMenu>("MainContextMenu", choices);
   RectFilled* menuCorner = mCurrentMenu->Add<RectFilled>("Funnyfart", glm::vec4(1, 0, 0, 1));

   // The menu MUST remain within my bounds
   UIConstraint::Options leftOptions;
   leftOptions.relationship = UIConstraint::GreaterThanOrEqual;
   mCurrentMenu->ConstrainLeftAlignedTo(this, 0.0, leftOptions);

   UIConstraint::Options topOption;
   topOption.relationship = UIConstraint::LessThanOrEqual;
   mCurrentMenu->ConstrainTopAlignedTo(this, 0.0, topOption);

   UIConstraint::Options rightOptions;
   rightOptions.relationship = UIConstraint::LessThanOrEqual;
   mCurrentMenu->ConstrainRightAlignedTo(this, 0.0, rightOptions);

   UIConstraint::Options bottomOptions;
   bottomOptions.relationship = UIConstraint::GreaterThanOrEqual;
   mCurrentMenu->ConstrainBottomAlignedTo(this, 0.0, bottomOptions);

   // "Menu Corner" is a 0-size point where the mouse cursor is.
   menuCorner->ConstrainWidth(10);
   menuCorner->ConstrainHeight(10);

   menuCorner->ConstrainLeftAlignedTo(this, x);
   menuCorner->ConstrainBottomAlignedTo(this, y);

   // Prefer the menu be placed with its top-left corner at "menuCorner", but if that's not possible,
   //    allow it to be placed with its bottom or right at "menuCorner".
   UIConstraint::Options mediumPriority;
   mediumPriority.priority = UIConstraint::MEDIUM_PRIORITY;

   UIConstraint::Options lowPriority;
   lowPriority.priority = UIConstraint::LOW_PRIORITY;

   menuCorner->ConstrainLeftAlignedTo(mCurrentMenu, 0.0, mediumPriority);
   menuCorner->ConstrainTopAlignedTo(mCurrentMenu, 0.0, mediumPriority);

   menuCorner->ConstrainRightAlignedTo(mCurrentMenu, 0.0, lowPriority);
   menuCorner->ConstrainBottomAlignedTo(mCurrentMenu, 0.0, lowPriority);

   mbIsShowingContextMenu = true;
}

//
// On mouse click, close context menu if it's open and consume the event.
//
UIElement::Action UIContextMenuParent::MouseClick(const MouseClickEvent&)
{
   if (!mbIsShowingContextMenu) {
      return Unhandled;
   }
   else {
      mbIsShowingContextMenu = false;
      mCurrentMenu->MarkForDeletion();
      return Handled;
   }
}

UIContextMenuParent::UIContextMenuParent(UIRoot* root, UIElement* parent, const std::string& name)
      : UIElement(root, parent, name)
      , mCurrentMenu(nullptr)
{
}


}  // Engine

}  // Cubeworld
