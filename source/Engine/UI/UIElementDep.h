//
// UIElementDep – Basic UI element. Stores its "frame," which gives the element
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

#include <glm/glm.hpp>
#include <rhea/variable.hpp>
#include <rhea/constraint.hpp>

#include <RGBLogger/Logger.h>
#include <RGBBinding/BindingProperty.h>
#include <RGBBinding/Observable.h>

#include "UIConstrainable.h"
#include "UIGestureRecognizer.h"
#include "../Core/Config.h"
#include "../Event/Event.h"
#include "../Event/EventManager.h"
#include "../Event/Receiver.h"
#include "../Event/InputEvent.h"
#include "../Graphics/VBO.h"
#include "../Graphics/FontManager.h"

namespace CubeWorld
{

namespace Engine
{

class UIRootDep; // Forward declare
class UIElementDep; // Forward declare

//
// Emitted whenever an element is added to the tree.
// Attached is a pointer to the new element.
//
class ElementAddedEvent : public Event<ElementAddedEvent>
{
public:
   ElementAddedEvent(UIElementDep* element) : element(element) {};

   UIElementDep* element;
};

//
// Emitted whenever an element is removed from the tree.
// Attached is a pointer to the new element.
//
class ElementRemovedEvent : public Event<ElementRemovedEvent>
{
public:
   ElementRemovedEvent(UIElementDep* element) : element(element) {};

   UIElementDep* element;
};

//
// Emitted whenever the UIRootDep is rebalanced.
//
class UIRebalancedEvent : public Event<UIRebalancedEvent>
{};

//
// UIElementDep extends EventManager, so that events can easily be passed down all the way to any leaf elements from
// a central EventManager.
//
class UIElementDep : public Engine::Receiver<UIElementDep>, public UIConstrainable
{
public:
   UIElementDep(UIRootDep *root, UIElementDep *parent, const std::string& name = "");
   virtual ~UIElementDep();

   virtual void InitFromJSON(const BindingProperty& data); // Override me :3
   virtual BindingProperty ConvertToJSON();                // Also override me :3

   //
   // Add a UIElementDep as a child of this one.
   //
   virtual UIElementDep* AddChild(std::unique_ptr<UIElementDep>&& element);

   //
   // Add a new element of type E as a child of this one.
   // Returns a pointer to the element, for referencing, configuring, etc.
   //
   template <typename E, typename ...Args>
   E* Add(Args ...args)
   {
      static_assert(std::is_base_of<UIElementDep, E>::value, "Only subclasses of UIElementDep may be added to a UIElementDep");

      return static_cast<E*>(AddChild(std::make_unique<E>(mpRoot, this, std::forward<Args>(args)...)));
   }

   //
   // Add a new gesture recognizer of type G to this element.
   // Returns a shared_pointer to the element, for referencing, configuring, etc.
   //
   template <typename G, typename ...Args>
   std::shared_ptr<G> CreateAndAddGestureRecognizer(Args ...args)
   {
      static_assert(std::is_base_of<UIGestureRecognizer, G>::value, "Only subclasses of UIGestureRecognizer should be added through here");

      return AddGestureRecognizer(std::make_shared<G>(this, std::forward<Args>(args)...));
   }

   //
   // Called whenever the UI is rebalanced or the active-ness of this element
   // is changed. Use this for setting up VBO data, responding to the current size, etc.
   //
   virtual void Redraw() {}

   //
   // Called whenever the UI is rebalanced.
   //
   void Receive(const UIRebalancedEvent&);

   //
   // Set whether an element is active or inactive.
   //
   virtual void SetActive(bool active);

   //
   // Get whether this element is active.
   //
   bool IsActive() { return mActive; }

   //
   // Element will be marked and removed at the end of the frame.
   //
   void MarkForDeletion() { mbDeleteAfterThisFrame = true; }

   void DestroyChild(UIElementDep* childToDestroy);

   bool IsMarkedForDeletion() const;

   //
   // Get pointer to parent of this element.
   //
   UIElementDep* GetParent() const;

   //
   // Get a const vector& of this element's children
   //
   const std::vector<std::unique_ptr<UIElementDep>>& GetChildren() { return mChildren; }

   //
   // Update the element, called once per frame with the time elapsed.
   // Useful for animations, resizing, and responding to input.
   //
   virtual void Update(TIMEDELTA dt);

   //
   // Create a set of constrains specifying that {other} is within {this}.
   // Specifically:
   //   - {this}.left <= {other}.left
   //   - {this}.right >= {other}.right
   //   - {this}.top >= {other}.top
   //   - {this}.bottom <= {other}.bottom
   //
   void Contains(UIElementDep* other, rhea::strength strength = rhea::strength::required());

   //
   // Returns whether the point [x, y] in pixel space
   // is contained within this element.
   //
   bool ContainsPoint(double x, double y) const;

   //
   // Used below.
   //
   struct DebugInfo {
      std::string name;
      std::string type = "UIElementDep";
      glm::tvec2<uint32_t> origin;
      glm::tvec2<uint32_t> size;
      double z, maxZ;
      bool active;
      std::vector<DebugInfo> children;
   };

   //
   // Returns debug info with this element's relevant properties
   //
   virtual DebugInfo GetDebugInfo(bool bRecursive = false);

   //
   // Logs a human-readable message with the debug info above. Leave output to null
   // to use default program output.
   //
   void LogDebugInfo(bool bRecursive = true, uint32_t indentLevel = 0);

   //
   // Called by the UIRootDep when mouse events happen.
   // These functions are performed from the front-most element to the furthest back,
   // allowing one element to "handle" the event and prevent it from propagating
   // further.
   //
   // Return Handled in order to prevent this event from propagating down, and
   // Unhandled if you don't care. Return Capture if you want to be the only
   // element that gets to handle the current mouse click+drag – this will
   // forward all Mouse events to you until the next MouseUpEvent.
   //
   enum Action { Handled, Unhandled, Capture };
   virtual Action MouseDown(const MouseDownEvent& evt);
   virtual Action MouseMove(const MouseMoveEvent& evt);
   virtual Action MouseUp(const MouseUpEvent& evt);
   virtual Action MouseClick(const MouseClickEvent&) { return Unhandled; }

   template<typename G>
   std::shared_ptr<G> AddGestureRecognizer(std::shared_ptr<G> recognizer)
   {
      mGestureRecognizers.push_back(recognizer);
      return recognizer;
   }

   Observables::Observable<bool>& OnActiveStateChanged() { return mActiveObservable; }

protected:
   // Sends a message when I become active or inactive
   Observables::Observable<bool> mActiveObservable;

   // Convenience DisposeBag for subclasses to use
   std::shared_ptr<Observables::DisposeBag> mBag;

   // Whether or not this element is considered active.
   // Adhering to this is up to the element itself.
   bool mActive = true;

   // If true, this element is MARKED FOR DEATH
   bool mbDeleteAfterThisFrame = false;

   // If true, this element will block mouse events to elements behind it.
   bool mbAbsorbsMouseEvents = false;

   // Children are owned by their parent elements.
   std::vector<std::unique_ptr<UIElementDep>> mChildren;

   // List of gesture recognizers on this element
   std::vector<std::shared_ptr<UIGestureRecognizer>> mGestureRecognizers;

   UIElementDep* mpParent;
};

//
// Operator-style constraint to specify that one object is above another.
//
rhea::linear_inequality operator>(UIElementDep& lhs, UIElementDep& rhs);
rhea::linear_inequality operator>(UIElementDep& lhs, UIFrame& rhs);
rhea::linear_inequality operator>(UIFrame& lhs, UIElementDep& rhs);
rhea::linear_inequality operator>(UIFrame& lhs, UIFrame& rhs);

}; // namespace Engine

}; // namespace CubeWorld
