//
// TextEntry.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include "UIElementDep.h"

#include <RGBBinding/Observable.h>

namespace CubeWorld
{

namespace Engine
{

struct Keystroke {
   int keyCode;
   int mods;
};

class TextEntry : public UIElementDep
{
public:
   TextEntry(UIRootDep* root, UIElementDep* parent, const std::string &name);
   void ProvideKeystrokeObservable(Observables::Observable<Keystroke>& keystrokes);

   Observables::Observable<std::string>& GetEnteredTextObservable() { return mEnteredTextObservable; }
   Observables::Observable<Keystroke>& GetKeystrokeObserver() { return mKeystrokeObservable; }

private:
   Observables::Observable<std::string> mEnteredTextObservable;
   Observables::Observable<Keystroke> mKeystrokeObservable;
   std::string mEnteredText;
};

} // namespace Engine

} // namespace CubeWorld
