//
// ToggleButtonVC.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include "ButtonVC.h"

#include <Shared/UI/Image.h>
#include <RGBBinding/Observable.h>

namespace CubeWorld
{

namespace Engine
{
   
using UI::Image;

class ToggleButtonVC : public ButtonVC
{
public:
   ToggleButtonVC(UIRoot* root, UIElement* parent, std::shared_ptr<Observable<bool>> isOn, Image::Options offImage, Image::Options onImage, const std::string& name = "");
   
protected:
   virtual void OnClick() override;

private:
   void Toggled(bool isOn);
   
   Image* mOffImage;
   Image* mOnImage;
   
   // If true, this toggle button is ENABLED
   std::shared_ptr<Observable<bool>> mIsOn;
};

}; // namespace Engine

}; // namespace CubeWorld
