//
// ConstrainerModel.h
//
// Model that keeps track of the UIElements you're editing and
//    the constraints between them.
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/UIElementDep.h>
#include <Shared/Helpers/JsonFileSync.h>

#include <RGBBinding/Observable.h>

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

using Engine::UIElementDep;

class ConstrainerModel
{
public:
   ConstrainerModel();

   Observables::Observable<UIElementDep*>& GetBaseElementObservable() { return mBaseElementObservable; }

private:
   Observables::Observable<UIElementDep*> mBaseElementObservable;

   // The base element of the UI we're editing
   UIElementDep* mpBaseElement;

   // Currently selected element, if any
   UIElementDep* mpSelectedElement;

   // Syncs the state of the editor's elements to a JSON file
   Shared::JsonFileSync mFileSyncer;
};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld
