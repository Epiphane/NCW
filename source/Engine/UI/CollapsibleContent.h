//
// CollapsibleContent.h
//
// This describes an element with a label and a ToggleButtonVC that
//    you can toggle to collapse/expand content beneath the label.
//
// This file created by the ELLIOT FISKE gang
//

#include "ToggleButtonVC.h"

#include "UIStackView.h"

namespace CubeWorld
{

namespace Engine
{

class CollapsibleContent : public UIElement
{
public:
   CollapsibleContent(UIRoot* root, UIElement* parent, const std::string &name);
   virtual ~CollapsibleContent() {}
   
   template<typename ElementType, typename ...Args>
   UIElement* AddContent(Args ...args)
   {
      if (mContent) {
         mContent->MarkForDeletion();
      }
      
      mContent = mContentParent->Add<ElementType>(std::forward<Args>(args)...);
         
      return mContent;
   }
   
   void ProvideCollapseStateSetter(Observables::Observable<bool>& collapser);
   
protected:
   ToggleButtonVC* mToggle;
   UIStackView* mContentParent;  // UIStackView that we will use to show/hide the content.
                                 // Note that UIStackViews do not reserve space for elements with IsActive() == false.
                                 //    We use this to set the size of the content to 0.
   
private:
   DECLARE_OBSERVABLE(bool, mExpanded, OnExpandedStateChange)
   UIElement* mContent = nullptr;
};

} // namespace Engine

} // namespace CubeWorld


