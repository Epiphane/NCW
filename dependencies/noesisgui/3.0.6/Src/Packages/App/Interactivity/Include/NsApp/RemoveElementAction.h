////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_REMOVEELEMENTACTION_H__
#define __APP_REMOVEELEMENTACTION_H__


#include <NsCore/Noesis.h>
#include <NsApp/InteractivityApi.h>
#include <NsApp/TargetedTriggerAction.h>
#include <NsGui/FrameworkElement.h>


namespace NoesisApp
{

NS_WARNING_PUSH
NS_CLANG_WARNING_DISABLE("-Wdocumentation")

////////////////////////////////////////////////////////////////////////////////////////////////////
/// An action that will remove the targeted element from the tree when invoked.
///
/// .. code-block:: xml
///
///    <Grid
///      xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
///      xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
///      xmlns:i="http://schemas.microsoft.com/expression/2010/interactivity"
///      xmlns:ei="http://schemas.microsoft.com/expression/2010/interactions">
///      <StackPanel x:Name="Item">
///        <Image Source="{Binding ProfileImage}"/>
///        <TextBlock Text="{Binding Description}"/>
///        <Button Content="Remove">
///          <i:Interaction.Triggers>
///            <i:EventTrigger EventName="Click">
///              <ei:RemoveElementAction TargetName="Item"/>
///            </i:EventTrigger>
///          </i:Interaction.Triggers>
///        </Button>
///      </StackPanel>
///    </Grid>
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_APP_INTERACTIVITY_API RemoveElementAction:
    public TargetedTriggerActionT<Noesis::FrameworkElement>
{
public:
    RemoveElementAction();
    ~RemoveElementAction();

protected:
    /// From Freezable
    //@{
    Noesis::Ptr<Noesis::Freezable> CreateInstanceCore() const override;
    //@}

    /// From TriggerAction
    //@{
    void Invoke(Noesis::BaseComponent* parameter) override;
    //}

    NS_DECLARE_REFLECTION(RemoveElementAction, TargetedTriggerActionT<Noesis::FrameworkElement>)
};

NS_WARNING_POP

}


#endif
