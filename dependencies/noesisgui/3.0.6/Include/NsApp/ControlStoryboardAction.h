////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_CONTROLSTORYBOARDACTION_H__
#define __APP_CONTROLSTORYBOARDACTION_H__


#include <NsCore/Noesis.h>
#include <NsCore/ReflectionDeclareEnum.h>
#include <NsApp/InteractivityApi.h>
#include <NsApp/StoryboardAction.h>


namespace NoesisApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
enum ControlStoryboardOption
{
    ControlStoryboardOption_Play,
    ControlStoryboardOption_Stop,
    ControlStoryboardOption_TogglePlayPause,
    ControlStoryboardOption_Pause,
    ControlStoryboardOption_Resume,
    ControlStoryboardOption_SkipToFill
};

NS_WARNING_PUSH
NS_CLANG_WARNING_DISABLE("-Wdocumentation")

////////////////////////////////////////////////////////////////////////////////////////////////////
/// An action that will change the state of a targeted storyboard when invoked.
///
/// .. code-block:: xml
///
///    <Grid
///      xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
///      xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
///      xmlns:i="http://schemas.microsoft.com/expression/2010/interactivity"
///      xmlns:ei="http://schemas.microsoft.com/expression/2010/interactions">
///      <Grid.Resources>
///        <Storyboard x:Key="BlinkText">
///          <ColorAnimation To="White" Duration="0:0:0.1" AutoReverse="True"
///            Storyboard.TargetProperty="Foreground.Color"
///            Storyboard.TargetName="StatusText"/>
///          </Storyboard>
///      </Grid.Resources>
///      <TextBlock x:Name="StatusText" Text="{Binding Status}" Foreground="Silver">
///        <i:Interaction.Triggers>
///          <ei:PropertyChangedTrigger Binding="{Binding Status}">
///             <ei:ControlStoryboardAction Storyboard="{StaticResource BlinkText}"/>
///          </ei:PropertyChangedTrigger>
///        </i:Interaction.Triggers>
///      </TextBlock>
///    </Grid>
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_APP_INTERACTIVITY_API ControlStoryboardAction: public StoryboardAction
{
public:
    ControlStoryboardAction();
    ~ControlStoryboardAction();

    /// Gets or sets the targeted Storyboard
    //@{
    ControlStoryboardOption GetControlStoryboardOption() const;
    void SetControlStoryboardOption(ControlStoryboardOption option);
    //@}

public:
    static const Noesis::DependencyProperty* ControlStoryboardOptionProperty;

protected:
    /// From Freezable
    //@{
    Noesis::Ptr<Noesis::Freezable> CreateInstanceCore() const override;
    void CloneCommonCore(const Freezable* source) override;
    //@}

    /// From TriggerAction
    //@{
    void Invoke(Noesis::BaseComponent* parameter) override;
    //@}

    NS_DECLARE_REFLECTION(ControlStoryboardAction, StoryboardAction)
};

NS_WARNING_POP

}

NS_DECLARE_REFLECTION_ENUM_EXPORT(NS_APP_INTERACTIVITY_API, NoesisApp::ControlStoryboardOption)


#endif
