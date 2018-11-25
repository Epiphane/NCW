//
//  UIStackView.h
//  Engine
//
//  Element that arranges its children in a list by automatically
//    adding constraints.
//
//  Created by Elliot Fiske on 11/15/18.
//

#pragma once

#include <Engine/UI/UIElement.h>

namespace CubeWorld
{
   
namespace Engine
{
   
class UIStackView : public UIElement
{
public:
   UIStackView(UIRoot *root, UIElement *parent, const std::string& name = "");

   virtual UIElement *AddChild(std::unique_ptr<UIElement> &&element) override;

   // Get the spacing between elements in the list.
   double GetOffset() const;
   
   // Set the spacing between elements in the list.
   void SetOffset(double offset);

   // Returns whether this StackView is stacked vertically.
   bool IsVertical() const;

   // Set the StackView's orientation.
   void SetVertical(bool vertical);

private:
   void RemakeConstraints();

   std::vector<UIConstraint> mConstraintsBetweenChildren;  ///< A list of all the constraints between children.
                                                           ///< First element is the constraint between the first and second children.
                                                           ///< Note that the size will always be children.size - 1

   UIConstraint mTopConstraint;     ///< Constrain the first child to my top
   UIConstraint mBottomConstraint;  ///< Constrain the last child to my butt

   double mOffset = 0.0;     ///< Spacing between elements
   bool mbVertical = true;   ///< Are elements arranged vertically or horizontally?
};

} // Engine
   
} // CubeWorld