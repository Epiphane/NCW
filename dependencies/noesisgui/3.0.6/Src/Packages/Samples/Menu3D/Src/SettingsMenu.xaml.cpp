////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "SettingsMenu.xaml.h"
#include "OptionSelector.xaml.h"

#include <NsCore/ReflectionImplementEmpty.h>
#include <NsGui/IntegrationAPI.h>


using namespace Menu3D;
using namespace Noesis;


////////////////////////////////////////////////////////////////////////////////////////////////////
SettingsMenu::SettingsMenu()
{
    InitializeComponent();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void SettingsMenu::InitializeComponent()
{
    GUI::LoadComponent(this, "SettingsMenu.xaml");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION_(Menu3D::SettingsMenu, "Menu3D.SettingsMenu")
