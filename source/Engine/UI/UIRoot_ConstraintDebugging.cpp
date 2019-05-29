//
// UIRoot_ConstraintDebugging.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "UIRoot_ConstraintDebugging.h"

#include <Engine/UI/UIRoot.h>

#include <Shared/UI/RectFilled.h>
#include <Shared/UI/Text.h>


namespace CubeWorld
{

namespace Engine
{

using UI::RectFilled;
using UI::Text;

UIRoot_ConstraintDebugging::UIRoot_ConstraintDebugging(UIRoot* root, UIElement* parent, const std::string &name)
      : UIElement(root, parent, name)
{
   mConstraintDebugHighlight = Add<RectFilled>("ConstraintDebugHighlighter", glm::vec4(1.0f, 0.41f, 0.71f, 1));

   mConstraintDebugLabelBG = Add<RectFilled>("ConstraintDebugLabelBG", glm::vec4(0, 0, 0, 1));
   mConstraintDebugLabel = mConstraintDebugLabelBG->Add<Text>(Text::Options{"X Elements Possible"});
   mConstraintDebugLabel->ConstrainWidthToContent();
   mConstraintDebugLabel->ConstrainHeightToContent();
   mConstraintDebugLabelBG->ConstrainTopAlignedTo(this);
   mConstraintDebugLabelBG->ConstrainLeftAlignedTo(this);

   mConstraintDebugLabel->ConstrainEqualBounds(mConstraintDebugLabelBG);

   mbAbsorbsMouseEvents = true;
}

UIElement::Action UIRoot_ConstraintDebugging::MouseMove(const MouseMoveEvent &evt)
{
   bool foundFrontmostElement = false;
   int numElementsUnderCursor = 0;

   // Find the frontmost UIElement under the mouse
   for (long ndx = 0; ndx < mpRoot->mElements.size(); ndx++) {
      UIElement* elem = mpRoot->mElements[ndx];

      if (elem == mpRoot->mContextMenuLayer || elem == mConstraintDebugHighlight || elem == this) {
         continue;
      }

      if (!foundFrontmostElement && elem->ContainsPoint(evt.x, evt.y)) {
         mpRoot->RemoveConstraintsForElement(mConstraintDebugHighlight);
         mConstraintDebugHighlight->ConstrainEqualBounds(elem);
         foundFrontmostElement = true;
      }

      if (elem->ContainsPoint(evt.x, evt.y)) {
         numElementsUnderCursor++;
      }
   }

   ((Text*) mConstraintDebugLabel)->SetText(Format::FormatString("%1 Possible Elements", numElementsUnderCursor));
}

UIElement::Action UIRoot_ConstraintDebugging::MouseDown(const MouseDownEvent &evt)
{
   for (long ndx = 0; ndx < mpRoot->mElements.size(); ndx++) {
      UIElement* elem = mpRoot->mElements[ndx];
      if (elem->ContainsPoint(evt.x, evt.y)) {
         elem->LogDebugInfo();
      }
   }

   return Handled;
}

} // namespace Engine

} // namespace CubeWorld
