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

ToggleButtonVC::ToggleButtonVC(UIRoot* root, UIElement* parent, Image::Options offImage, Image::Options onImage, const std::string& name)
   : ButtonVC(root, parent, name)
{
   mOffImage = Add<Image>(offImage);
   mOnImage = Add<Image>(onImage);
   
   
}

void ToggleButtonVC::OnClick()
{
   ButtonVC::OnClick();
}
   
void ToggleButtonVC::Toggled(bool isOn)
{
   mOffImage->SetActive(!isOn);
   mOnImage->SetActive(isOn);
}

}; // namespace Engine

}; // namespace CubeWorld
