//
// ConstrainerModel.h
//
// Model that keeps track of the UIElements you're editing and
//    the constraints between them.
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/UIElement.h>
#include <Shared/Helpers/JsonFileSync.h>

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

using Engine::UIElement;
using Engine::UIConstraint;

class ConstrainerModel
{
public:
   ConstrainerModel();

   UIElement* GetBaseElement() const;
   void SetBaseElement(UIElement* mpBaseElement);

   void SetModelUpdatedCallback(const std::function<void()> &mModelUpdatedCallback);
   
   size_t GetTotalNumElements();

private:
   // Helper function that returns how many elements are in my hierarchy
   //    (plus one for me)
   size_t TotalNumberOfElementsInSubtree(UIElement& subtreeRoot);
   
   std::function<void(void)> mModelUpdatedCallback;

   // All the constraints created for the UI we're editing
   std::vector<UIConstraint> constraints;

   // The base element of the UI we're editing
   UIElement* mpBaseElement;
   
   // Currently selected element, if any
   UIElement* mpSelectedElement;

   // Syncs the state of the editor's elements to a JSON file
   Shared::JsonFileSync mFileSyncer;
};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld
