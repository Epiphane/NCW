////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __INVENTORY_DRAGADORNERBEHAVIOR_H__
#define __INVENTORY_DRAGADORNERBEHAVIOR_H__


#include <NsCore/Noesis.h>
#include <NsGui/FrameworkElement.h>
#include <NsApp/Behavior.h>


namespace Noesis
{
class DependencyProperty;
struct DragEventArgs;
struct Point;
}

namespace Inventory
{

////////////////////////////////////////////////////////////////////////////////////////////////////
class DragAdornerBehavior final: public NoesisApp::BehaviorT<Noesis::FrameworkElement>
{
public:
    const Noesis::Point& GetDragStartOffset() const;
    void SetDragStartOffset(const Noesis::Point& offset);

    float GetDraggedItemX() const;
    float GetDraggedItemY() const;

public:
    static const Noesis::DependencyProperty* DragStartOffsetProperty;
    static const Noesis::DependencyProperty* DraggedItemXProperty;
    static const Noesis::DependencyProperty* DraggedItemYProperty;

protected:
    Noesis::Ptr<Freezable> CreateInstanceCore() const override;
    void OnAttached() override;
    void OnDetaching() override;

private:
    void OnDragOver(Noesis::BaseComponent* sender, const Noesis::DragEventArgs& e);
    void OnDrop(Noesis::BaseComponent* sender, const Noesis::DragEventArgs& e);

    void SetDraggedItemX(float x);
    void SetDraggedItemY(float y);

    NS_DECLARE_REFLECTION(DragAdornerBehavior, Behavior)
};

}


#endif
