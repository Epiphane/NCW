////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "MainWindow.xaml.h"

#include <NsCore/Version.h>
#include <NsGui/Keyboard.h>
#include <NsGui/FreezableCollection.h>
#include <NsGui/Transform.h>
#include <NsGui/TransformGroup.h>
#include <NsGui/ScaleTransform.h>
#include <NsGui/TranslateTransform.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/CommandBinding.h>
#include <NsGui/CompositeTransform3D.h>
#include <NsGui/ObservableCollection.h>
#include <NsGui/UICollection.h>
#include <NsGui/TextBlock.h>
#include <NsGui/ItemsControl.h>
#include <NsApp/Application.h>


using namespace Noesis;
using namespace NoesisApp;
using namespace XamlPlayer;


////////////////////////////////////////////////////////////////////////////////////////////////////
XamlPlayer::MainWindow::MainWindow(): mContainer(0), mContainerScale(0), mContainerTranslate(0),
    mTransform3D(0), mZoom(1.0f), mDragging(false), mRotating(false)
{
    StrCopy(mActiveFilename, sizeof(mActiveFilename), "");
    Initialized() += MakeDelegate(this, &MainWindow::OnInitialized);
    Loaded() += MakeDelegate(this, &MainWindow::OnLoaded);
    InitializeComponent();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
XamlPlayer::MainWindow::~MainWindow()
{
    // restore default error handler so following errors don't try to use this
    SetErrorHandler(mDefaultErrorHandler);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::LoadXAML(const char* filename)
{
    mContainer->SetContent((BaseComponent*)0);
    mErrorList->Clear();

    Ptr<FrameworkElement> content = GUI::LoadXaml<FrameworkElement>(filename);
    mContainer->SetContent(content);

    if (!StrEquals(filename, "Content.xaml"))
    {
        UpdateTitle(filename);
        StrCopy(mActiveFilename, sizeof(mActiveFilename), filename);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::OnPreviewMouseRightButtonDown(const MouseButtonEventArgs& e)
{
    ParentClass::OnPreviewMouseRightButtonDown(e);

    ModifierKeys m = GetKeyboard()->GetModifiers();

    if (IsFileLoaded() && (m & (ModifierKeys_Control | ModifierKeys_Alt)) != 0)
    {
        CaptureMouse();

        if (m & ModifierKeys_Control)
        {
            mDragging = true;
        }
        else if (m & ModifierKeys_Alt)
        {
            mRotating = true;
        }

        mDraggingLastPosition = e.position;
        e.handled = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::OnPreviewMouseRightButtonUp(const MouseButtonEventArgs& e)
{
    ParentClass::OnPreviewMouseRightButtonUp(e);

    if (mDragging || mRotating)
    {
        ReleaseMouseCapture();

        mDragging = false;
        mRotating = false;

        e.handled = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::OnPreviewMouseMove(const MouseEventArgs& e)
{
    ParentClass::OnPreviewMouseMove(e);

    if (mDragging)
    {
        float x = mContainerTranslate->GetX();
        float y = mContainerTranslate->GetY();

        Point delta = e.position - mDraggingLastPosition;
        mDraggingLastPosition = e.position;

        mContainerTranslate->SetX(x + delta.x);
        mContainerTranslate->SetY(y + delta.y);

        e.handled = true;
    }

    if (mRotating)
    {
        float x = mTransform3D->GetRotationY();
        float y = mTransform3D->GetRotationX();

        Point delta = e.position - mDraggingLastPosition;
        mDraggingLastPosition = e.position;

        mTransform3D->SetRotationY(x - delta.x * 0.5f);
        mTransform3D->SetRotationX(y + delta.y * 0.5f);

        e.handled = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::OnPreviewMouseWheel(const MouseWheelEventArgs& e)
{
    ParentClass::OnPreviewMouseWheel(e);

    if (IsFileLoaded() && (GetKeyboard()->GetModifiers() & ModifierKeys_Control) != 0)
    {
        mZoom = Clip(mZoom * (e.wheelRotation > 0 ? 1.05f : 0.952381f), 0.01f, 100.0f);

        float width = mContainer->GetActualWidth();
        float height = mContainer->GetActualHeight();

        Point center(width * 0.5f, height * 0.5f);
        Point point = mContainer->PointFromScreen(e.position);
        Point pointScaled = center + (point - center) * mZoom;
        Point offset = e.position - pointScaled;

        mContainerScale->SetScaleX(mZoom);
        mContainerScale->SetScaleY(mZoom);

        if (IsZero(mTransform3D->GetRotationX()) && IsZero(mTransform3D->GetRotationY()))
        {
            mContainerTranslate->SetX(offset.x);
            mContainerTranslate->SetY(offset.y);
        }

        e.handled = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::OnFileDropped(const char* filename)
{
    if (StrCaseEndsWith(filename, ".xaml"))
    {
        LoadXAML(filename);
        Reset();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::ErrorHandler(const char* filename, uint32_t line, const char* desc,
    bool fatal)
{
    MainWindow* window = (MainWindow*)Application::Current()->GetMainWindow();
    window->mErrorList->Add(Boxing::Box(desc));
    window->mDefaultErrorHandler(filename, line, desc, fatal);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::InitializeComponent()
{
    GUI::LoadComponent(this, "XamlPlayer/MainWindow.xaml");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::UpdateTitle(const char* filename)
{
    char title[512] = "XamlPlayer ";

    if (!StrStartsWith(GetBuildVersion(), "0.0.0"))
    {
        StrAppend(title, sizeof(title), "v");
        StrAppend(title, sizeof(title), GetBuildVersion());
    }

    if (filename != nullptr)
    {
        StrAppend(title, sizeof(title), " - ");
        StrAppend(title, sizeof(title), filename);
    }

    SetTitle(title);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool XamlPlayer::MainWindow::IsFileLoaded() const
{
    return !StrIsNullOrEmpty(mActiveFilename);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::Reset()
{
    mZoom = 1.0f;

    mContainerScale->SetScaleX(1.0f);
    mContainerScale->SetScaleY(1.0f);
    mContainerTranslate->SetX(0.0f);
    mContainerTranslate->SetY(0.0f);

    mTransform3D->SetRotationX(0.0f);
    mTransform3D->SetRotationY(0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::OnInitialized(BaseComponent*, const EventArgs&)
{
    UpdateTitle(nullptr);

    // Error list
    ItemsControl* errors = FindName<ItemsControl>("Errors");
    NS_ASSERT(errors != 0);
    mErrorList = *new ObservableCollection<BaseComponent>();
    errors->SetItemsSource(mErrorList);
    mDefaultErrorHandler = SetErrorHandler(ErrorHandler);

    CommandBindingCollection* commandBindings = GetCommandBindings();
    NS_ASSERT(commandBindings->Count() == 2);

    // Reset command
    CommandBinding* resetCmd = commandBindings->Get(0);
    resetCmd->CanExecute() += MakeDelegate(this, &MainWindow::OnCanExecuteReset);
    resetCmd->Executed() += MakeDelegate(this, &MainWindow::OnExecuteReset);

    CommandBinding* updateCmd = commandBindings->Get(1);
    updateCmd->CanExecute() += MakeDelegate(this, &MainWindow::OnCanExecuteUpdate);
    updateCmd->Executed() += MakeDelegate(this, &MainWindow::OnExecuteUpdate);

    mContainer = FindName<ContentControl>("Container");
    NS_ASSERT(mContainer != 0);

    TransformGroup* group = (TransformGroup*)mContainer->GetRenderTransform();
    TransformCollection* children = group->GetChildren();
    NS_ASSERT(children->Count() == 2);

    mContainerScale = (ScaleTransform*)children->Get(0);
    NS_ASSERT(mContainerScale != 0);

    mContainerTranslate = (TranslateTransform*)children->Get(1);
    NS_ASSERT(mContainerTranslate != 0);

    mTransform3D = (CompositeTransform3D*)mContainer->GetTransform3D();
    NS_ASSERT(mTransform3D != 0);

    LoadXAML("Content.xaml");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::OnLoaded(BaseComponent*, const Noesis::RoutedEventArgs&)
{
    if (StrIsNullOrEmpty(mActiveFilename))
    {
        PreviewToolbar();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::OnCanExecuteReset(BaseComponent*, const CanExecuteRoutedEventArgs& e)
{
    e.canExecute = true;
    e.handled = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::OnExecuteReset(BaseComponent*, const ExecutedRoutedEventArgs& e)
{
    Reset();
    e.handled = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::OnCanExecuteUpdate(BaseComponent*, const CanExecuteRoutedEventArgs& e)
{
    e.canExecute = IsFileLoaded();
    e.handled = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void XamlPlayer::MainWindow::OnExecuteUpdate(BaseComponent*, const ExecutedRoutedEventArgs&)
{
    LoadXAML(mActiveFilename);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION_(XamlPlayer::MainWindow, "XamlPlayer.MainWindow")
