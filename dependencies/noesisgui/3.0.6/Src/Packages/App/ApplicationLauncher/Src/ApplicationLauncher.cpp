////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/ApplicationLauncher.h>
#include <NsApp/Application.h>
#include <NsApp/LocalXamlProvider.h>
#include <NsApp/LocalTextureProvider.h>
#include <NsApp/LocalFontProvider.h>
#include <NsApp/EmbeddedXamlProvider.h>
#include <NsApp/EmbeddedFontProvider.h>
#include <NsApp/ThemeProviders.h>
#include <NsApp/Window.h>
#include <NsGui/IntegrationAPI.h>
#include <NsCore/StringUtils.h>


using namespace Noesis;
using namespace NoesisApp;


namespace
{

#ifdef NS_PROFILE
// Dripicons V2 by Amit Jakhu
// https://github.com/amitjakhu/dripicons
#include "dripicons-v2.ttf.bin.h"
#include "ModeNine.ttf.bin.h"
#include "StatsOverlay.xaml.bin.h"
#endif

}

////////////////////////////////////////////////////////////////////////////////////////////////////
ApplicationLauncher::ApplicationLauncher()
{
    StrCopy(mAppFile, sizeof(mAppFile), "");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ApplicationLauncher::~ApplicationLauncher()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationLauncher::SetApplicationFile(const char* filename)
{
    StrCopy(mAppFile, sizeof(mAppFile), filename);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationLauncher::DisableInspector()
{
    GUI::DisableInspector();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationLauncher::OnStartUp()
{
#ifdef NS_PROFILE
    // XAML provider
    EmbeddedXaml xamls[] =
    {
         { "_StatsOverlay.xaml", StatsOverlay_xaml }
    };

    Ptr<EmbeddedXamlProvider> xamlProvider = *new EmbeddedXamlProvider(xamls, GetXamlProvider());

    // Font provider
    EmbeddedFont fonts[] =
    {
        { "_", dripicons_v2_ttf },
        { "_", ModeNine_ttf }
    };

    Ptr<EmbeddedFontProvider> fontProvider = *new EmbeddedFontProvider(fonts, GetFontProvider());
    Ptr<TextureProvider> textureProvider = GetTextureProvider();
#else
    Ptr<XamlProvider> xamlProvider = GetXamlProvider();
    Ptr<FontProvider> fontProvider = GetFontProvider();
    Ptr<TextureProvider> textureProvider = GetTextureProvider();
#endif

    // Theme
    NoesisApp::SetThemeProviders(xamlProvider, fontProvider, textureProvider);

    // Load application file
    if (StrIsNullOrEmpty(mAppFile))
    {
        NS_FATAL("Application file not defined");
    }

    mApplication = DynamicPtrCast<Application>(GUI::LoadXaml(mAppFile));
    if (mApplication == 0)
    {
        NS_FATAL("File '%s' does not define a valid application file.", mAppFile);
    }

    mApplication->Init(GetDisplay(), GetArguments());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationLauncher::OnTick(double time)
{
    mApplication->Tick(time);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicationLauncher::OnExit()
{
    mApplication.Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<XamlProvider> ApplicationLauncher::GetXamlProvider() const
{
    return *new LocalXamlProvider("");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<TextureProvider> ApplicationLauncher::GetTextureProvider() const
{
    return *new LocalTextureProvider("");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Ptr<FontProvider> ApplicationLauncher::GetFontProvider() const
{
    return *new LocalFontProvider("");
}
