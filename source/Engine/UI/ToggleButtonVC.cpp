//
// ToggleButtonVC.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "ToggleButtonVC.h"


namespace CubeWorld
{

namespace Engine
{

ToggleButtonVC::ToggleButtonVC(UIRoot* root, UIElement* parent, const Observable<bool>& isOn, Image::Options offImage, Image::Options onImage, const std::string& name)
   : ButtonVC(root, parent, name)
   , mIsOn(isOn)
{
   mOffImage = Add<Image>(offImage);
   mOnImage = Add<Image>(onImage);
   
   mIsOn.AddCallback(std::bind(&ToggleButtonVC::Toggled, this, std::placeholders::_1));
}
   
void ToggleButtonVC::Toggled(bool isOn)
{
   mOffImage->SetActive(!isOn);
   mOnImage->SetActive(isOn);
}

}; // namespace Engine

}; // namespace CubeWorld
