//
//  UISerializationHelper.h
//  Engine
//
//  Created by Elliot Fiske on 11/19/18.
//

#pragma once

#include <string>
#include <map>

#include <Shared/Helpers/JsonHelper.h>

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
   Maybe<ElementsByName> CreateUIFromJSONData(nlohmann::json data, UIRoot* pRoot, UIElement* pParent);
   Maybe<ElementsByName> CreateUIFromJSONFile(const std::string& filename, UIRoot* pRoot, UIElement* pParent);
   
   // Serialization public-facing methods
   nlohmann::json CreateJSONFromUI(UIElement *element, const std::vector<UIConstraint>& constraints);

private:
   // Deserialization helpers
   UIConstraint::Target ConstraintTargetFromString(std::string name);
   Maybe<void> ParseUIElement(nlohmann::json element, UIRoot* pRoot, UIElement* pParent, ElementsByName* elementMapOut);
   Maybe<void> ParseConstraints(nlohmann::json constraints, UIRoot* pRoot, const ElementsByName &elementsMap);
   
   // Serialization helpers
   void SerializeUIElement(UIElement* element, nlohmann::json* dataIn);
   std::string StringFromConstraintTarget(UIConstraint::Target target);
   nlohmann::json SerializeConstraints(UIElement* element, const std::vector<UIConstraint>& constraints);
};
   
} // CubeWorld
   
} // Engine
