//
// CollapsibleTreeVC.h
//
// UIElement that holds a tree of items. You can click each
//    item to expand the children underneath it in the tree.
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/UIRoot.h>
#include <Engine/UI/UIStackView.h>

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

using Engine::UIStackView;
using Engine::UIElement;

class CollapsibleTreeVC : public UIElement
{
public:
   CollapsibleTreeVC(Engine::UIRoot* root, UIElement* parent, const std::string& name);

private:
   UIStackView *mStackView;

   // List of items in the tree view. All of these items are direct children of mStackView.
//   std::vector<CollapsibleTreeItem*> mItems;
};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld
