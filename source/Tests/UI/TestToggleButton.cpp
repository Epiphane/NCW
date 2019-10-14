#include "../catch.h"

#include "Mocks/Mocks_UIElement.h"

#include <Engine/UI/ToggleButtonVC.h>

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld;
using namespace Observables;

using Engine::ToggleButtonVC;
using UI::Image;

SCENARIO("Toggle buttons send the correct messages when clicked or force-toggled") {

	GIVEN("A toggle button with an Observer attached") {
		Test::MockInput input;
		std::unique_ptr<Engine::UIRoot> dummyRoot = CreateDummyUIRoot(input);
		Engine::UIRoot* root = dummyRoot.get();
		std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();

		ToggleButtonVC* button = dummyRoot->Add<ToggleButtonVC>(Image::Options(), Image::Options(), "ToggleButtonDummy");

		std::vector<bool> toggles;

		button->GetToggleObservable() >>
			ToContainer<std::vector<bool>>(toggles, myBag);

		WHEN("The toggle button receives mouse events") {
			THEN("the toggle button should send messages from its OnToggled observable.") {
				MockClick(root, button);

				// ToggleButton defaults to false, so toggling once should give us one true
				std::vector<bool> expected = { true };
				CHECK(expected == toggles);

				MockClick(root, button);
				MockClick(root, button);
				MockClick(root, button);

				expected = { true, false, true, false };
				CHECK(expected == toggles);
			}
		}

		AND_WHEN("the toggle button receives piped messages") {
			Observable<bool> toggleMeister;
			toggleMeister >> button->GetToggleObservable();

			toggleMeister.SendMessage(false);
			toggleMeister.SendMessage(false);
			toggleMeister.SendMessage(true);

			THEN("it should also send messages from its OnToggled observable.") {
				std::vector<bool> expectedToggles = { false, false, true };
				CHECK(expectedToggles == toggles);
			}
		}

		AND_WHEN("piped messages are combined with mouse events") {
			Observable<bool> toggleMeister;
			toggleMeister >> button->GetToggleObservable();

			toggleMeister.SendMessage(false);
			MockClick(root, button);
			toggleMeister.SendMessage(false);
			toggleMeister.SendMessage(true);
			MockClick(root, button);
			MockClick(root, button);
			MockClick(root, button);
			toggleMeister.SendMessage(false);
			toggleMeister.SendMessage(false);

			THEN("the OnToggled observable sends all the toggles out.") {
				std::vector<bool> expectedToggles = { false, true, false, true, false, true, false, false, false };
				CHECK(expectedToggles == toggles);
			}
		}

} // GIVEN

} // SCENARIO
