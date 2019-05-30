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
   
const static std::map<UIStackView::AlignItemsBy, UIConstraint::Target> CONSTRAINT_MAPPING = {
   {UIStackView::Left,    UIConstraint::Left},
   {UIStackView::Right,   UIConstraint::Right}, 
   {UIStackView::CenterX, UIConstraint::CenterX},
   {UIStackView::Top,     UIConstraint::Top},
   {UIStackView::Bottom,  UIConstraint::Bottom}, 
   {UIStackView::CenterY, UIConstraint::CenterY}
};

UIStackView::UIStackView(UIRoot *root, UIElement *parent, const std::string& name)
   : UIElement(root, parent, name)
   , mAlignItemsBy(0)
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
 * Destroy any constraints previously generated by this UIStackView
 */
void UIStackView::DestroyOldConstraints() {
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
   
   for (UIConstraint constraint : mAlignmentConstraints) {
      if (mpRoot->GetConstraint(constraint.GetName())) {
         mpRoot->RemoveConstraint(constraint.GetName());
      }
   }
   
   mConstraintsBetweenChildren.clear();
   mAlignmentConstraints.clear();
}
   
/**
 *  Destroy and remove all my existing constraints and make new ones based
 *    on my current parameters.
 *
 *  TODO: Use a 'dirty' flag so this only gets called once. Would need some kind
 *          of lifecycle hook like "UIRoot is about to solve constraints."
 */
void UIStackView::RemakeConstraints()
{
   DestroyOldConstraints();

   if (mChildren.size() == 0) {
      return;
   }

   if (mbVertical) {
      UIConstraint::Options topOptions;
      topOptions.customNameConnector = "_topAlignedWithStackView_";
      mTopConstraint    = mChildren.front()->ConstrainTopAlignedTo(this, 0.0, topOptions);

      UIConstraint::Options bottomOptions;
      bottomOptions.customNameConnector = "_bottomAlignedWithStackView_";
      mBottomConstraint = mChildren.back()->ConstrainBottomAlignedTo(this, 0.0, bottomOptions);

      for (size_t ndx = 1; ndx < mChildren.size(); ndx++) {
         UIConstraint::Options options;
         options.customNameConnector = "_belowInStackView_";
         UIConstraint newConstraint = mChildren[ndx]->ConstrainBelow(mChildren[ndx-1].get(), mOffset, options);
         mConstraintsBetweenChildren.push_back(newConstraint);
      }
   }
   else {
      mTopConstraint    = mChildren.front()->ConstrainLeftAlignedTo(this);
      mBottomConstraint = mChildren.back()->ConstrainRightAlignedTo(this);

      for (size_t ndx = 1; ndx < mChildren.size(); ndx++) {
         UIConstraint::Options options;
         options.customNameConnector = "_rightOfInStackView_";
         UIConstraint newConstraint = mChildren[ndx]->ConstrainToRightOf(mChildren[ndx-1].get(), mOffset, options);
         mConstraintsBetweenChildren.push_back(newConstraint);
      }
   }
   
   CreateConstraintsForItemAlignment();
}

void UIStackView::CreateConstraintsForItemAlignment() {
   for (size_t ndx = 0; ndx < AlignItemsBy::Count; ndx++) {
      AlignItemsBy axisToAlignItems = (AlignItemsBy)ndx;
      if (mAlignItemsBy.test(axisToAlignItems)) {
         if (CONSTRAINT_MAPPING.find(axisToAlignItems) == CONSTRAINT_MAPPING.end()) {
            assert(false && "Invalid constraint target!");
            continue;
         }
         
         UIConstraint::Target target = CONSTRAINT_MAPPING.at(axisToAlignItems);
         for (size_t child = 0; child < mChildren.size(); child++) {
            UIConstraint::Options options;
            options.customNameConnector = "_aligned" + UIConstraint::StringFromConstraintTarget(target) + "ToStackView_";
            
            UIConstraint newConstraint(this, mChildren[child].get(), target, target, options);
            mpRoot->AddConstraint(newConstraint);
            mAlignmentConstraints.push_back(newConstraint);
         }
      }
   }
}
   
void UIStackView::SetAlignItemsBy(AlignItemsBy alignmentFlag) {
   mAlignItemsBy.set(alignmentFlag);
   RemakeConstraints();
}

void UIStackView::UnsetAlignItemsBy(AlignItemsBy alignmentFlag) {
   mAlignItemsBy.reset(alignmentFlag);
   RemakeConstraints();
}

} // Engine
   
} // CubeWorld
