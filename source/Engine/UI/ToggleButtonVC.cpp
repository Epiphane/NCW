//
// ToggleButtonVC.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "ToggleButtonVC.h"

#include <RGBBinding/ObservableBasicOperations.h>

namespace CubeWorld
{

namespace Engine
{
   
using namespace Observables;

ToggleButtonVC::ToggleButtonVC(UIRoot* root, UIElement* parent, Image::Options offImage, Image::Options onImage, const std::string& name)
   : ButtonVC(root, parent, name)
{
   // TODO-EF: The ContentSize of a ToggleButton (and a Button in general) should be
   //             equal to the size of its background image.
   //
   //          And/or if there is a text label on the button, that should also inform
   //             the content size somehow. I dunno I'll cross that bridge when I get to it.
   mOffImage = Add<Image>(offImage);
   mOnImage = Add<Image>(onImage);

   mToggled.MessageProducer() >>
      StartWith(false) >>
      Distinct() >>
      OnMessage<bool>([&](bool isOn) {
         mToggleState = isOn;
         
         mOnImage->SetActive(isOn);
         mOffImage->SetActive(!isOn);
      }, mBag);
   
   OnClick() >>
     OnMessage<UIGestureRecognizer::Message_GestureState>([&](auto m) {
        mToggled.SendMessage(!mToggleState);
     }, mBag);
}

void ToggleButtonVC::ProvideToggleSetter(Observable<bool>& toggler)
{
   // TODO-EF: It might be cool to syntactically sugar this into something like:
   //             public MessageSink ToggleStateSink() { 
   //                return mToggled.GetMessageSink();
   //             }
   //
   //             ...
   //
   //             SomeOtherObservable >> mToggleButton.ToggleStateSink();
   toggler >>
      OnMessage<bool>([&](bool toggled) {
         mToggled.SendMessage(toggled);
      }, mBag);
}

}; // namespace Engine

}; // namespace CubeWorld
