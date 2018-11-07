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

#include "../Core/Config.h"
#include "../Event/Event.h"
#include "../Event/EventManager.h"
#include "../Event/Receiver.h"
#include "../Event/InputEvent.h"
#include "../Graphics/VBO.h"
#include "../Graphics/FontManager.h"
#include "../UI/UIConstrainable.h"

namespace CubeWorld
{

namespace Engine
{

/**
 * Data for one vertex. We build up an array of vertices by
 *  walking through the UI heirarchy.
 */
struct VertexData
{
   VertexData() {
      std::memset(this, 0, sizeof(VertexData));
   };
   
   glm::vec2 position, uv;
//   glm::vec4 color; // Maybe someday
};

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
   UIElement(UIRoot* root, UIElement* parent);

   //
   // Add a UIElement as a child of this one.
   //
   UIElement* AddChild(std::unique_ptr<UIElement>&& element);

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
   // Update the element, called once per frame with the time elapsed.
   // Useful for animations, resizing, and responding to input.
   //
   virtual void Update(TIMEDELTA dt);

   //
   // Create a constraint specifying that the height of {this} > {other}
   //
   void ConstrainAbove(UIElement* other, rhea::strength strength = rhea::strength::required());

   //
   // Add a named constraint to this element. Element will report this to its mpRoot.
   //
   void AddConstraint(std::string nameKey, const rhea::constraint& constraint);

   //
   // Returns whether the point [x, y] in pixel space
   // is contained within this element.
   //
   bool ContainsPoint(double x, double y);

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
   virtual Action MouseDown(const MouseDownEvent&) { return Unhandled; }
   virtual Action MouseMove(const MouseMoveEvent&) { return Unhandled; }
   virtual Action MouseUp(const MouseUpEvent&) { return Unhandled; }
   virtual Action MouseClick(const MouseClickEvent&) { return Unhandled; }

protected:
   
   // Whether or not this element is considered active.
   // Adhering to this is up to the element itself.
   bool mActive;

   // Map where from a constraint's name to its value
   std::map<std::string, rhea::constraint> mConstraints;

   // Children are owned by their parent elements.
   std::vector<std::unique_ptr<UIElement>> mChildren;

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
