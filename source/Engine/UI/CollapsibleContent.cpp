//
// CollapsibleContent.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "CollapsibleContent.h"

#include "UIRoot.h"

#include <RGBBinding/ObservableBasicOperations.h>
#include <Shared/Helpers/Asset.h>

namespace CubeWorld
{

namespace Engine
{

CollapsibleContent::CollapsibleContent(UIRoot* root, UIElement* parent, const std::string &name)
      : UIElement(root, parent, name)
{
   Image::Options offImage{Asset::Image("EditorIcons.png"), "button_right"};
   Image::Options onImage{Asset::Image("EditorIcons.png"), "button_down"};
   mToggle = Add<ToggleButtonVC>(offImage, onImage, name + "Toggle");
   
   mContentParent = Add<UIStackView>(name + "Content");
   mContentParent->ConstrainEqualBounds(this);
   
   mToggle->OnToggled() >>
      Observables::OnMessage<bool>([=](bool newToggleState) {
         if (mContent) {
            mContent->SetActive(newToggleState);
         }
      }, mBag);
}
   
void CollapsibleContent::ProvideCollapseStateSetter(Observables::Observable<bool>& collapser) 
{
   collapser >>
      Observables::OnMessage<bool>([=](bool newToggleState) {
         
      }, mBag);
}
   
void CollapsibleContent::SetContent(std::unique_ptr<UIElement> element)
{
   if (mContent) {
      mContent->MarkForDeletion();
   }
   
   mContent = mContentParent->AddChild(std::move(element));
}

} // namespace Engine

} // namespace CubeWorld
