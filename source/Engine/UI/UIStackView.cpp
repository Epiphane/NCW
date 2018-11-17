//
//  UIStackView.cpp
//
//  Element that arranges its children in a list by automatically
//    adding constraints.
//
//  Created by Elliot Fiske on 11/15/18.
//

#include "UIStackView.h"

#include <Engine/UI/UIElement.h>
#include <Engine/UI/UIRoot.h>

namespace CubeWorld
{
   
namespace Engine
{

UIStackView::UIStackView(UIRoot *root, UIElement *parent, const std::string& name)
    : UIElement(root, parent, name)
{
}


UIElement *UIStackView::AddChild(std::unique_ptr<UIElement> &&element)
{
   UIElement* result = UIElement::AddChild(std::move(element));

   RemakeConstraints();

   return result;
}

/**
 * @return The spacing between elements in the list.
 */
double UIStackView::GetOffset() const
{
   return mOffset;
}

/**
 * @param offset New spacing between elements in the list.
 */
void UIStackView::SetOffset(double offset)
{
   UIStackView::mOffset = offset;
   RemakeConstraints();
}

/**
 * @return True if this stack is vertically oriented.
 */
bool UIStackView::IsVertical() const
{
   return mbVertical;
}

/**
 * @param vertical Sets whether the list is vertically oriented.
 */
void UIStackView::SetVertical(bool vertical)
{
   UIStackView::mbVertical = vertical;
   RemakeConstraints();
}

/**
 *  Destroy and remove all my existing constraints and make new ones based
 *    on my current parameters.
 */
void UIStackView::RemakeConstraints()
{
   if (mpRoot->GetConstraint(mTopConstraint.GetName())) {
      mpRoot->RemoveConstraint(mTopConstraint.GetName());
   }

   if (mpRoot->GetConstraint(mBottomConstraint.GetName())) {
      mpRoot->RemoveConstraint(mBottomConstraint.GetName());
   } 

   for (UIConstraint constraint : mConstraintsBetweenChildren) {
      if (mpRoot->GetConstraint(constraint.GetName())) {
         mpRoot->RemoveConstraint(constraint.GetName());
      }
   }
   
   mConstraintsBetweenChildren.clear();

   if (mChildren.size() == 0) {
      return;
   }

   if (mbVertical) {
      UIConstraint::Options topOptions;
      topOptions.mCustomNameConnector = "_topAlignedWithStackView_";
      mTopConstraint    = mChildren.front()->ConstrainTopAlignedTo(this, 0.0, topOptions);

      UIConstraint::Options bottomOptions;
      bottomOptions.mCustomNameConnector = "_bottomAlignedWithStackView_";
      mBottomConstraint = mChildren.back()->ConstrainBottomAlignedTo(this, 0.0, bottomOptions);

      for (int ndx = 1; ndx < mChildren.size(); ndx++) {
         UIConstraint::Options options;
         options.mCustomNameConnector = "_belowInStackView_";
         UIConstraint newConstraint = mChildren[ndx]->ConstrainBelow(mChildren[ndx-1].get(), mOffset, options);
         mConstraintsBetweenChildren.push_back(newConstraint);
      }
   }
   else {
      mTopConstraint    = mChildren.front()->ConstrainLeftAlignedTo(this);
      mBottomConstraint = mChildren.back()->ConstrainRightAlignedTo(this);

      for (int ndx = 1; ndx < mChildren.size(); ndx++) {
         UIConstraint::Options options;
         options.mCustomNameConnector = "_rightOfInStackView_";
         UIConstraint newConstraint = mChildren[ndx]->ConstrainToRightOf(mChildren[ndx-1].get(), mOffset);
      }
   }
}

} // Engine
   
} // CubeWorld
