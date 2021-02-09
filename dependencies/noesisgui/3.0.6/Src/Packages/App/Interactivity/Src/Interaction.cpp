////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/Interaction.h>
#include <NsApp/BehaviorCollection.h>
#include <NsApp/TriggerCollection.h>
#include <NsGui/FrameworkElement.h>
#include <NsGui/DependencyData.h>


using namespace NoesisApp;


////////////////////////////////////////////////////////////////////////////////////////////////////
BehaviorCollection* Interaction::GetBehaviors(const Noesis::DependencyObject* obj)
{
    if (obj == 0)
    {
        NS_ERROR("Can't get Interaction.Behaviors, element is null");
        return 0;
    }

    Noesis::Ptr<BehaviorCollection> behaviors = obj->GetValue<Noesis::Ptr<BehaviorCollection>>(BehaviorsProperty);
    if (behaviors == 0)
    {
        behaviors = *new BehaviorCollection();
        Noesis::DependencyObject* associatedObject = const_cast<Noesis::DependencyObject*>(obj);
        associatedObject->SetValue<Noesis::Ptr<BehaviorCollection>>(BehaviorsProperty, behaviors);
    }

    return behaviors;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NoesisApp::TriggerCollection* Interaction::GetTriggers(const Noesis::DependencyObject* obj)
{
    if (obj == 0)
    {
        NS_ERROR("Can't get Interaction.Triggers, element is null");
        return 0;
    }

    Noesis::Ptr<TriggerCollection> triggers = obj->GetValue<Noesis::Ptr<TriggerCollection>>(TriggersProperty);
    if (triggers == 0)
    {
        triggers = *new TriggerCollection();
        Noesis::DependencyObject* associatedObject = const_cast<Noesis::DependencyObject*>(obj);
        associatedObject->SetValue<Noesis::Ptr<TriggerCollection>>(TriggersProperty, triggers);
    }

    return triggers;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnBehaviorsChanged(Noesis::DependencyObject* d,
    const Noesis::DependencyPropertyChangedEventArgs& e)
{
    BehaviorCollection* oldBehaviors = ((const Noesis::Ptr<BehaviorCollection>*)e.oldValue)->GetPtr();
    if (oldBehaviors != 0)
    {
        NS_ASSERT(oldBehaviors->GetAssociatedObject() == d);
        oldBehaviors->Detach();
    }

    BehaviorCollection* newBehaviors = ((const Noesis::Ptr<BehaviorCollection>*)e.newValue)->GetPtr();
    if (newBehaviors != 0)
    {
        NS_ASSERT(newBehaviors->GetAssociatedObject() == 0);
        newBehaviors->Attach(d);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static void OnTriggersChanged(Noesis::DependencyObject* d,
    const Noesis::DependencyPropertyChangedEventArgs& e)
{
    TriggerCollection* oldTriggers = ((const Noesis::Ptr<TriggerCollection>*)e.oldValue)->GetPtr();
    if (oldTriggers != 0)
    {
        NS_ASSERT(oldTriggers->GetAssociatedObject() == d);
        oldTriggers->Detach();
    }

    TriggerCollection* newTriggers = ((const Noesis::Ptr<TriggerCollection>*)e.newValue)->GetPtr();
    if (newTriggers != 0)
    {
        NS_ASSERT(newTriggers->GetAssociatedObject() == 0);
        newTriggers->Attach(d);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION(Interaction, "NoesisApp.Interaction")
{
    Noesis::DependencyData* data = NsMeta<Noesis::DependencyData>(Noesis::TypeOf<SelfClass>());
    data->RegisterProperty<Noesis::Ptr<BehaviorCollection>>(BehaviorsProperty, "Behaviors",
        Noesis::PropertyMetadata::Create(Noesis::Ptr<BehaviorCollection>(), OnBehaviorsChanged));
    data->RegisterProperty<Noesis::Ptr<TriggerCollection>>(TriggersProperty, "Triggers",
        Noesis::PropertyMetadata::Create(Noesis::Ptr<TriggerCollection>(), OnTriggersChanged));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const Noesis::DependencyProperty* Interaction::BehaviorsProperty;
const Noesis::DependencyProperty* Interaction::TriggersProperty;
