//
// TextEntry.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include "UIElement.h"

#include <RGBBinding/Observable.h>

namespace CubeWorld
{

namespace Engine
{

struct Keystroke {
   int keyCode;
   int mods;
};

class TextEntry : public UIElement
{
public:
   TextEntry(UIRoot* root, UIElement* parent, const std::string &name);
   void ProvideKeystrokeObservable(Observables::Observable<Keystroke>& keystrokes);

   Observables::Observable<std::string>& OnInputChanging();

private:
   Observables::ObservableInternal<std::string> mEnteredTextObservable;
   std::string mEnteredText;
};

} // namespace Engine

} // namespace CubeWorld
