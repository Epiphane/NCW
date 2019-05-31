#include "../catch.h"

#include <Engine/UI/TextEntry.h>

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld::Observables;
using CubeWorld::Engine::TextEntry;
using CubeWorld::Engine::Keystroke;

TEST_CASE( "Basic text entry behavior" ) {
   TextEntry* text = new TextEntry(nullptr, nullptr, "TextEntryTestDummy");

   std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();
   ObservableInternal<Keystroke> fakeKeystrokes;
   
   std::string enteredText;
   
   text->ProvideKeystrokeObservable(fakeKeystrokes.OnChanged());
   text->OnInputChanging() >>
      OnMessage<std::string>([&](std::string newString) {
         enteredText = newString;
      }, myBag);
   
   fakeKeystrokes.SendMessage({'h', 0});
   fakeKeystrokes.SendMessage({'i', 0});
   
   CHECK( enteredText == "hi" );
}

