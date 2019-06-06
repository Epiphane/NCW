//
// TextEntry.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "TextEntry.h"
#include <RGBBinding/ObservableBasicOperations.h>


namespace CubeWorld
{

namespace Engine
{

TextEntry::TextEntry(UIRoot* root, UIElement* parent, const std::string &name) 
   : UIElement(root, parent, name)
{
}

void TextEntry::ProvideKeystrokeObservable(Observables::Observable<Keystroke>& keystrokes)
{
   keystrokes >>
      Observables::OnMessage<Keystroke>([&](Keystroke key) {
         mEnteredText += (char) key.keyCode;
         mEnteredTextObservable.SendMessage(mEnteredText);
      }, mBag);
}

Observables::Observable<std::string>& TextEntry::OnInputChanging()
{
   return mEnteredTextObservable.MessageProducer();
}


} // namespace Engine

} // namespace CubeWorld
