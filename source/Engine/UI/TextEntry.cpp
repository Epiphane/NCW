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

TextEntry::TextEntry(UIRootDep* root, UIElementDep* parent, const std::string &name)
   : UIElementDep(root, parent, name)
{
   mKeystrokeObservable >>
      Observables::OnMessage<Keystroke>([&](Keystroke key) {
         mEnteredText += (char) key.keyCode;
         mEnteredTextObservable.SendMessage(mEnteredText);
      }, mBag);
}

} // namespace Engine

} // namespace CubeWorld
