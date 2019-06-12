//
// CollapsibleContent.h
//
// This describes an element with a label and a ToggleButtonVC that
//    you can toggle to collapse/expand content beneath the label.
//
// This file created by the ELLIOT FISKE gang
//

#include "ToggleButtonVC.h"

namespace CubeWorld
{

namespace Engine
{

class UIStackView; // Forward declare

class CollapsibleContent : public UIElement
{
public:
   CollapsibleContent(UIRoot* root, UIElement* parent, const std::string &name);
   
   // Give the VC content to collapse. Content should have its width and height constrained.
   void SetContent(std::unique_ptr<UIElement> element);
   
   void ProvideCollapseStateSetter(Observables::Observable<bool>& collapser);
   
protected:
   ToggleButtonVC* mToggle;
   
private:
   DECLARE_OBSERVABLE(bool, mCollapsed, OnCollapsedStateChange)
   UIStackView* mContentParent;  // UIStackView that we will use to show/hide the content.
                                 // Note that UIStackViews do not reserve space for elements with IsActive() == false.
                                 //    We use this to set the size of the content to 0.
   UIElement* mContent = nullptr;
};

} // namespace Engine

} // namespace CubeWorld


