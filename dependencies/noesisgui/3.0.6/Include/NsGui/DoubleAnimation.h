////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __GUI_DOUBLEANIMATION_H__
#define __GUI_DOUBLEANIMATION_H__


#include <NsCore/Noesis.h>
#include <NsCore/ReflectionDeclare.h>
#include <NsGui/BaseAnimation.h>
#include <NsGui/AnimationApi.h>


namespace Noesis
{

template<class T> class Nullable;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Animates the value of a *Float* property between two target values using linear interpolation
/// over a specified *Duration*.
///
/// http://msdn.microsoft.com/en-us/library/system.windows.media.animation.doubleanimation.aspx
////////////////////////////////////////////////////////////////////////////////////////////////////
class NS_GUI_ANIMATION_API DoubleAnimation final: public BaseAnimation
{
public:
    /// Gets the type of value this animation generates
    const Type* GetTargetPropertyType() const override;

    /// Gets or sets the total amount by which the animation changes its starting value
    //@{
    const Nullable<float>& GetBy() const;
    void SetBy(const Nullable<float>& by);
    //@}

    /// Gets or sets the animation's starting value
    //@{
    const Nullable<float>& GetFrom() const;
    void SetFrom(const Nullable<float>& from);
    //@}

    /// Gets or sets the animation's ending value
    //@{
    const Nullable<float>& GetTo() const;
    void SetTo(const Nullable<float>& to);
    //@}

    /// From Freezable
    //@{
    Ptr<DoubleAnimation> Clone() const;
    Ptr<DoubleAnimation> CloneCurrentValue() const;
    //@}

public:
    /// Dependency properties
    //@{
    static const DependencyProperty* ByProperty;
    static const DependencyProperty* FromProperty;
    static const DependencyProperty* ToProperty;
    //@}

private:
    /// From DependencyObject
    //@{
    bool OnPropertyChanged(const DependencyPropertyChangedEventArgs& args) override;
    //@}

    /// From Freezable
    //@{
    void CloneCommonCore(const Freezable* source) override;
    Ptr<Freezable> CreateInstanceCore() const override;
    //@}

    /// From AnimationTimeline
    //@{
    Ptr<BaseComponent> GetAnimatedValue(BaseComponent* defaultOrigin, 
        BaseComponent* defaultDestination, AnimationClock* clock) override;
    Ptr<AnimationTimeline> CreateTransitionFrom() const override;
    Ptr<AnimationTimeline> CreateTransitionTo() const override;
    //@}

private:
    NS_DECLARE_REFLECTION(DoubleAnimation, BaseAnimation)
};

}

#endif
