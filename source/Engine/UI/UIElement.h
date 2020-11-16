//
// UIElement – Basic UI element. Stores its "frame," which gives the element
//                its coordinates and size.
//
// By Thomas Steinke
//

#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <RGBLogger/Logger.h>
#include <RGBBinding/BindingProperty.h>

#include "../Core/Config.h"
#include "../Event/Event.h"
#include "../Event/Receiver.h"
#include "../Event/InputEvent.h"

namespace CubeWorld
{

namespace Engine
{

class UIRoot;

//
// UIElement extends EventManager, so that events can easily be passed down all the way to any leaf elements from
// a central EventManager.
//
class UIElement : public Engine::Receiver<UIElement>
{
public:
    UIElement(UIRoot* root, UIElement* parent, const std::string& name = "");
    virtual ~UIElement();

    //
    // Add a UIElement as a child of this one.
    //
    virtual UIElement* AddChild(std::unique_ptr<UIElement>&& element);

    //
    // Add a new element of type E as a child of this one.
    // Returns a pointer to the element, for referencing, configuring, etc.
    //
    template <typename E, typename ...Args>
    E* Add(Args ...args)
    {
        static_assert(std::is_base_of<UIElement, E>::value, "Only subclasses of UIElement may be added to a UIElement");

        return static_cast<E*>(AddChild(std::make_unique<E>(mpRoot, this, std::forward<Args>(args)...)));
    }

    //
    // Set whether an element is active or inactive.
    //
    virtual void SetActive(bool active);

    //
    // Get whether this element is active.
    //
    bool IsActive() { return mActive; }

    void DestroyChild(UIElement* childToDestroy);

    //
    // Get pointer to parent of this element.
    //
    UIElement* GetParent() const;

    //
    // Get a const vector& of this element's children
    //
    const std::vector<std::unique_ptr<UIElement>>& GetChildren() { return mChildren; }

    //
    // Update the element, called once per frame with the time elapsed.
    // Useful for animations, resizing, and responding to input.
    //
    virtual void Update(TIMEDELTA dt);

protected:
    std::string mName;

    // Whether or not this element is considered active.
    // Adhering to this is up to the element itself.
    bool mActive = true;

    // Children are owned by their parent elements.
    std::vector<std::unique_ptr<UIElement>> mChildren;

    UIRoot* mpRoot;
    UIElement* mpParent;
};

}; // namespace Engine

}; // namespace CubeWorld
