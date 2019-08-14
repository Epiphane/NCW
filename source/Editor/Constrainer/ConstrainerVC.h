// By Thomas Steinke

#pragma once

#include <memory>

#include <Engine/Event/EventManager.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/StateWindow.h>
#include <Shared/Helpers/JsonFileSync.h>

#include "ConstrainerModel.h"

#include <Engine/UI/CollapsibleTreeVC.h>

#include "../Controls.h"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

using Engine::CollapsibleTreeVC;
   
class ConstrainerVC : public Engine::UIRoot
{
public:
   ConstrainerVC(Engine::Input* input, const Controls::Options& options);

   //
   // Called every time this editor is reactivated.
   //
   void Start();

private:
   void ParseUIElementTitles(UIElement& baseElement);

   ConstrainerModel mModel;
   
   CollapsibleTreeVC *mElementList;
};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld
