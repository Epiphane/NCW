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

class UIElementDep; // Forward declare
class UIRootDep;

typedef std::map<std::string, UIElementDep*> ElementsByName;

class UISerializationHelper {
public:
   // Deserialization public-facing methods
   Maybe<ElementsByName> CreateUIFromJSONData(const BindingProperty& data, UIRootDep* pRoot, UIElementDep* pParent);
   Maybe<ElementsByName> CreateUIFromJSONFile(const std::string& filename, UIRootDep* pRoot, UIElementDep* pParent);

   // Serialization public-facing methods
   BindingProperty CreateJSONFromUI(UIElementDep *element, const std::vector<UIConstraint>& constraints);

private:
   // Deserialization helpers
   Maybe<void> ParseUIElement(const BindingProperty& element, UIRootDep* pRoot, UIElementDep* pParent, ElementsByName* elementMapOut);
   Maybe<void> ParseConstraints(const BindingProperty& constraints, UIRootDep* pRoot, const ElementsByName &elementsMap);

   // Serialization helpers
   BindingProperty SerializeConstraints(const std::vector<UIConstraint>& constraints);
};

} // CubeWorld

} // Engine
