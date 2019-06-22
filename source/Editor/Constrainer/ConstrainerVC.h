// By Thomas Steinke

#pragma once

#include <memory>

#include <Engine/Event/EventManager.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/StateWindow.h>
#include <Shared/Helpers/JsonFileSync.h>

#include "ConstrainerModel.h"

#include "CollapsibleTreeVC.h"

#include "../Controls.h"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

class ConstrainerVC : public Engine::UIRoot
{
public:
   ConstrainerVC(Engine::Input* input, const Controls::Options& options);

   //
   // Called every time this editor is reactivated.
   //
   void Start();

private:
   // Called when the Model has updates (i.e. if the user edits UI, or if the underlying JSON changes)
   void ModelUpdated();
//   std::unique_ptr<CollapsibleTreeItem> ParseUIElementTitles(UIElement& baseElement);

   Engine::EventManager mEvents;

   ConstrainerModel mModel;
   
   CollapsibleTreeVC *mElementList;
};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld
