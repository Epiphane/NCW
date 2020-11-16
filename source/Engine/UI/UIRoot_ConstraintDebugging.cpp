//
// UIRoot_ConstraintDebugging.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "UIRoot_ConstraintDebugging.h"

#include <Engine/UI/UIRootDep.h>

#include <Shared/UI/RectFilled.h>
#include <Shared/UI/Text.h>


namespace CubeWorld
{

namespace Engine
{

using UI::RectFilled;
using UI::Text;

UIRoot_ConstraintDebugging::UIRoot_ConstraintDebugging(UIRootDep* root, UIElementDep* parent, const std::string &name)
      : UIElementDep(root, parent, name)
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

UIElementDep::Action UIRoot_ConstraintDebugging::MouseMove(const MouseMoveEvent &evt)
{
   bool foundFrontmostElement = false;
   int numElementsUnderCursor = 0;

   // Find the frontmost UIElementDep under the mouse
   for (size_t ndx = 0; ndx < mpRoot->mElements.size(); ndx++) {
      UIElementDep* elem = mpRoot->mElements[ndx];

      if (elem == mpRoot->mContextMenuLayer || elem == mConstraintDebugHighlight || elem == this) {
         continue;
      }

      if (!foundFrontmostElement && elem->ContainsPoint(evt.x, evt.y)) {
         mpRoot->RemoveConstraintsForElement(mConstraintDebugHighlight);
         mConstraintDebugHighlight->ConstrainEqualBounds(elem);
         foundFrontmostElement = true;
      }

      if (elem->ContainsPoint(evt.x, evt.y) && dynamic_cast<Text*>(elem) != nullptr) {
         numElementsUnderCursor++;
      }
   }

   ((Text*) mConstraintDebugLabel)->SetText(FormatString("{num} Possible Elements", numElementsUnderCursor));
   return Unhandled;
}

UIElementDep::Action UIRoot_ConstraintDebugging::MouseDown(const MouseDownEvent &evt)
{
   for (size_t ndx = 0; ndx < mpRoot->mElements.size(); ndx++) {
      UIElementDep* elem = mpRoot->mElements[ndx];
      if (elem->ContainsPoint(evt.x, evt.y) && dynamic_cast<Text*>(elem) != nullptr) {
         elem->LogDebugInfo();
      }
   }

   return Handled;
}

} // namespace Engine

} // namespace CubeWorld
