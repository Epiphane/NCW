//
// TextEntry.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include "UIElement.h"
#include "Focusable.h"

#include <Shared/UI/Text.h>
#include <Shared/UI/RectFilled.h>

#include <RGBBinding/Observable.h>

namespace CubeWorld
{

namespace Engine
{

class TextEntry : public UIElement, Focusable
{
public:
   TextEntry(UIRoot* root, UIElement* parent, const std::string &name);

   Observables::Observable<std::u32string>& GetEnteredTextObservable() { return mEnteredTextObservable; }
   Observables::Observable<unsigned int>& GetKeystrokeObserver() { return mKeystrokeObservable; }

private:
   UI::Text* mDisplayedText;
   UI::RectFilled* mCursor;
   UI::RectFilled* mSelectionRect;

   Observables::Observable<std::u32string> mEnteredTextObservable;
   Observables::Observable<unsigned int> mKeystrokeObservable;
   std::u32string mEnteredText;
};

} // namespace Engine

} // namespace CubeWorld
