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
   mKeystrokeObservable >>
      Observables::OnMessage<unsigned int>([&](unsigned int codepoint) {
         mEnteredText += (char) codepoint;
         mEnteredTextObservable.SendMessage(mEnteredText);
      }, mBag);
}

} // namespace Engine

} // namespace CubeWorld
