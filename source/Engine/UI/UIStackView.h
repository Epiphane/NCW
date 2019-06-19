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

#include <bitset>

#include <Engine/UI/UIElement.h>

namespace CubeWorld
{
   
namespace Engine
{
   
class UIStackView : public UIElement
{
public:
   //
   // Enum that lets you specify more constraints to be added to stacked elements.
   //    For instance, specifying (Left | Right) means both the left and right edge 
   //    of the stacked elements will be aligned to the Stackview.
   //
   enum AlignItemsBy {
      Left, Right, CenterX,
      Top, Bottom, CenterY,
      Count
   };
   
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
   
   // Accessors to the alignItemsBy field
   void SetAlignItemsBy(AlignItemsBy alignmentFlag);
   void UnsetAlignItemsBy(AlignItemsBy alignmentFlag);

private:
   void DestroyOldConstraints();
   void CreateChildConstraints(const std::vector<UIElement*>& activeChildren);
   
   void RemakeConstraints();
   
   // Helper function that creates constraints to honor SetAlignItemsBy
   void CreateConstraintsForItemAlignment(const std::vector<UIElement*>& activeChildren);

   std::vector<UIConstraint> mConstraintsBetweenChildren;  ///< A list of all the constraints between children.
                                                           ///< First element is the constraint between the first and second children.
                                                           ///< Note that the size will always be children.size - 1
   
   std::vector<UIConstraint> mAlignmentConstraints; ///< Extra constraints to align elements to the Stackview.
   std::bitset<AlignItemsBy::Count> mAlignItemsBy;  ///< Bitflags you can set to add more constraints to stacked elements. See AlignItemsBy for more info.
   

   UIConstraint mTopConstraint;     ///< Constrain the first child to my top
   UIConstraint mBottomConstraint;  ///< Constrain the last child to my butt
   UIConstraint mEmptyConstraint;   ///< If the StackView is empty, constrain its main axis to 0

   double mOffset = 0.0;     ///< Spacing between elements
   bool mbVertical = true;   ///< Are elements arranged vertically or horizontally?
};

} // Engine
   
} // CubeWorld
