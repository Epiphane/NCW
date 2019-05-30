//
//  UISerializationHelper.h
//  Engine
//
//  Created by Elliot Fiske on 11/19/18.
//

#pragma once

#include <string>
#include <map>

#include <RGBBinding/BindingProperty.h>

#include "UIConstraint.h"

namespace CubeWorld
{
   
namespace Engine
{

class UIElement; // Forward declare
class UIRoot;

typedef std::map<std::string, UIElement*> ElementsByName;
   
class UISerializationHelper {
public:
   // Deserialization public-facing methods
   Maybe<ElementsByName> CreateUIFromJSONData(const BindingProperty& data, UIRoot* pRoot, UIElement* pParent);
   Maybe<ElementsByName> CreateUIFromJSONFile(const std::string& filename, UIRoot* pRoot, UIElement* pParent);
   
   // Serialization public-facing methods
   BindingProperty CreateJSONFromUI(UIElement *element, const std::vector<UIConstraint>& constraints);

private:
   // Deserialization helpers
   Maybe<void> ParseUIElement(const BindingProperty& element, UIRoot* pRoot, UIElement* pParent, ElementsByName* elementMapOut);
   Maybe<void> ParseConstraints(const BindingProperty& constraints, UIRoot* pRoot, const ElementsByName &elementsMap);
   
   // Serialization helpers
   BindingProperty SerializeConstraints(const std::vector<UIConstraint>& constraints);
};
   
} // CubeWorld
   
} // Engine
