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

#include <glm/glm.hpp>
#include <rhea/variable.hpp>
#include <rhea/constraint.hpp>

#include <RGBDesignPatterns/DereferenceIterator.h>
#include <RGBLogger/Logger.h>
#include <RGBBinding/tmp/json.hpp>

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

class UIRoot; // Forward declare
class UIElement; // Forward declare

//
// Emitted whenever an element is added to the tree.
// Attached is a pointer to the new element.
//
class ElementAddedEvent : public Event<ElementAddedEvent>
{
public:
   ElementAddedEvent(UIElement* element) : element(element) {};

   UIElement* element;
};

//
// Emitted whenever an element is removed from the tree.
// Attached is a pointer to the new element.
//
class ElementRemovedEvent : public Event<ElementRemovedEvent>
{
public:
   ElementRemovedEvent(UIElement* element) : element(element) {};

   UIElement* element;
};

//
// Emitted whenever the UIRoot is rebalanced.
//
class UIRebalancedEvent : public Event<UIRebalancedEvent>
{};

//
// UIElement extends EventManager, so that events can easily be passed down all the way to any leaf elements from
// a central EventManager.
//
class UIElement : public Engine::Receiver<UIElement>, public UIConstrainable
{
public:
   UIElement(UIRoot *root, UIElement *parent, const std::string& name = "");
   virtual ~UIElement();

   virtual void InitFromJSON(nlohmann::json data);      // Override me :3
   virtual void ConvertToJSON(nlohmann::json *outJson); // Also override me :3

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
   // Add a new gesture recognizer of type G to this element.
   // Returns a shared_pointer to the element, for referencing, configuring, etc.
   //
   template <typename G, typename ...Args>
   G* CreateAndAddGestureRecognizer(Args ...args)
   {
      static_assert(std::is_base_of<UIGestureRecognizer, G>::value, "Only subclasses of UIGestureRecognizer should be added through here");
      
      return static_cast<G*>(AddGestureRecognizer(std::make_unique<G>(this, std::forward<Args>(args)...)));
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

   void DestroyChild(UIElement* childToDestroy);

   bool IsMarkedForDeletion() const;

   //
   // Get pointer to parent of this element.
   //
   UIElement* GetParent() const;

   typedef Shared::DereferenceIterator<std::vector<std::unique_ptr<UIElement>>::iterator> ChildIterator;
   
   ChildIterator BeginChildren() {
      return Shared::MakeDereferenceIterator(mChildren.begin());
   }
   
   ChildIterator EndChildren() {
      return Shared::MakeDereferenceIterator(mChildren.end());
   }

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
   void Contains(UIElement* other, rhea::strength strength = rhea::strength::required());

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
      std::string type = "UIElement";
      glm::tvec2<uint32_t> origin;
      glm::tvec2<uint32_t> size;
      double z, maxZ;
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
   void LogDebugInfo(bool bRecursive = true, Logger::LogManager* output = nullptr, uint32_t indentLevel = 0);

public:
   //
   // Called by the UIRoot when mouse events happen.
   // These functions are performed from the front-most element to the furthest back,
   // allowing one element to "handle" the event and prevent it from propagating
   // further.
   //
   // Return Handled in order to prevent this event from propagating down, and
   // Unhandled if you don't care.
   //
   enum Action { Handled, Unhandled };
   virtual Action MouseDown(const MouseDownEvent& evt);
   virtual Action MouseMove(const MouseMoveEvent& evt);
   virtual Action MouseUp(const MouseUpEvent& evt);
   virtual Action MouseClick(const MouseClickEvent&) { return Unhandled; }
   
   UIGestureRecognizer* AddGestureRecognizer(std::shared_ptr<UIGestureRecognizer> recognizer);

protected:

   // Whether or not this element is considered active.
   // Adhering to this is up to the element itself.
   bool mActive;

   // If true, this element is MARKED FOR DEATH
   bool mbDeleteAfterThisFrame = false;

   // Children are owned by their parent elements.
   std::vector<std::unique_ptr<UIElement>> mChildren;
   
   // List of gesture recognizers on this element
   std::vector<std::shared_ptr<UIGestureRecognizer>> mGestureRecognizers;

   UIElement* mpParent;
};

//
// Operator-style constraint to specify that one object is above another.
//
rhea::linear_inequality operator>(UIElement& lhs, UIElement& rhs);
rhea::linear_inequality operator>(UIElement& lhs, UIFrame& rhs);
rhea::linear_inequality operator>(UIFrame& lhs, UIElement& rhs);
rhea::linear_inequality operator>(UIFrame& lhs, UIFrame& rhs);

}; // namespace Engine

}; // namespace CubeWorld
