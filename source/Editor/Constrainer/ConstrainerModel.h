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

#include <RGBBinding/Observable.h>

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

   Observables::Observable<UIElement*>& GetBaseElementObservable() { return mBaseElementObservable; }
   
private:
   Observables::Observable<UIElement*> mBaseElementObservable;

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
