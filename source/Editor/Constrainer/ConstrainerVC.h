// By Thomas Steinke

#pragma once

#include <memory>

#include <Engine/Event/EventManager.h>
#include <Engine/UI/UIRootDep.h>
#include <Shared/UI/StateWindow.h>
#include <Shared/Helpers/JsonFileSync.h>

#include "ConstrainerModel.h"

#include <Engine/UI/CollapsibleTreeVC.h>

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

using Engine::CollapsibleTreeVC;

class ConstrainerVC : public Engine::UIRootDep
{
public:
   ConstrainerVC(Engine::Input& input);

   //
   // Called every time this editor is reactivated.
   //
   void Start();

private:
   void ParseUIElementTitles(UIElementDep& baseElement);

   ConstrainerModel mModel;

   CollapsibleTreeVC *mElementList;
};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld
