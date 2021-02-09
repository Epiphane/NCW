////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_ENTRYPOINT_H__
#define __APP_ENTRYPOINT_H__


#include <NsCore/Noesis.h>


////////////////////////////////////////////////////////////////////////////////////////////////////
// Common main entry point for all Noesis applications
////////////////////////////////////////////////////////////////////////////////////////////////////
int NsMain(int argc, char** argv);


////////////////////////////////////////////////////////////////////////////////////////////////////
/// Platform dependent application entry point
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NS_PLATFORM_WINDOWS_DESKTOP
    #ifdef _CONSOLE
        int main(int argc, char** argv)
        {
            return NsMain(argc, argv);
        }
    #else
        #include <stdlib.h>
        struct HINSTANCE__;
        typedef HINSTANCE__* HINSTANCE;

        extern "C" int __stdcall WinMain(HINSTANCE, HINSTANCE, char*, int)
        {
            return NsMain(__argc, __argv);
        }
    #endif

#elif defined(NS_PLATFORM_IPHONE)
    #import <UIKit/UIKit.h>
    #import <CoreFoundation/CFURL.h>

    static int __argc;
    static char** __argv;

    @interface AppDelegate: NSObject<UIApplicationDelegate>
    @end

    @implementation AppDelegate

    - (void)applicationDidFinishLaunching:(UIApplication *)application
    {
        [[UIApplication sharedApplication] setStatusBarHidden:YES];
        [self performSelector:@selector(performInit:) withObject:nil afterDelay:0.2f];
    }

    - (void)performInit:(id)object
    {
        NsMain(__argc, __argv);
    }

    @end

    int main(int argc, char** argv)
    {
        __argc = argc;
        __argv = argv;

        // Set working directory to main bundle
        CFBundleRef bundle = CFBundleGetMainBundle();
        if (bundle)
        {
            CFURLRef url = CFBundleCopyBundleURL(bundle);
            if (url)
            {
                char uri[PATH_MAX];
                if (CFURLGetFileSystemRepresentation(url, true, (UInt8*)uri, PATH_MAX))
                {
                    chdir(uri);
                }

                CFRelease(url);
            }
        }

        return UIApplicationMain(argc, argv, nil, @"AppDelegate");
    }

#elif defined(NS_PLATFORM_OSX)
    int main(int argc, char** argv)
    {
        return NsMain(argc, argv);
    }

#elif defined(NS_PLATFORM_ANDROID)
    #include <NsApp/Display.h>
    #include <android_native_app_glue.h>

    void android_main(android_app* app)
    {
        // Store current app environment
        NoesisApp::Display::SetPrivateData(app);

        const char* argv[] = { "/system/bin/app_process" };
        NsMain(1, (char**)argv);

        // We need to kill the process because if we don't do that the native dynamic library will
        // be reused for the next activity and all our static variables will have garbage
        exit(0);
    }

#elif defined(NS_PLATFORM_LINUX) || defined(NS_PLATFORM_EMSCRIPTEN)
    int main(int argc, char** argv)
    {
        return NsMain(argc, argv);
    }

#elif defined(NS_PLATFORM_WINRT)
    using namespace Windows::Foundation;
    using namespace Windows::ApplicationModel::Activation;
    using namespace Windows::ApplicationModel::Core;
    using namespace Windows::UI::Core;

    ref class ViewProvider sealed: public IFrameworkView
    {
    public:
        virtual void Initialize(CoreApplicationView^ applicationView)
        {
            applicationView->Activated += ref new TypedEventHandler<CoreApplicationView^,
                IActivatedEventArgs^>(this, &ViewProvider::OnActivated);
        }

        virtual void Uninitialize() {}
        virtual void SetWindow(CoreWindow^ window) {}
        virtual void Load(Platform::String^ entryPoint) {}

        virtual void Run()
        {
            char* argv[] = { "App.exe" };
            NsMain(1, argv);
        }

    private:
        void OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
        {
            CoreWindow::GetForCurrentThread()->Activate();
        }
    };

    ref class ViewProviderFactory: IFrameworkViewSource
    {
    public:
        virtual IFrameworkView^ CreateView()
        {
            return ref new ViewProvider();
        }
    };

    [Platform::MTAThread]
    int main(Platform::Array<Platform::String^>^ argv)
    {
        IFrameworkViewSource^ viewProviderFactory = ref new ViewProviderFactory();
        CoreApplication::Run(viewProviderFactory);
        return 0;
    }

#else
    #error Unknown platform
#endif


#endif
