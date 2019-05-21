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
   ToggleButtonVC(UIRoot* root, UIElement* parent, Image::Options offImage, Image::Options onImage, const std::string& name = "");
   
protected:
   virtual void OnClick() override;

private:
   void Toggled(bool isOn);
   
   Image* mOffImage;
   Image* mOnImage;
};

}; // namespace Engine

}; // namespace CubeWorld
