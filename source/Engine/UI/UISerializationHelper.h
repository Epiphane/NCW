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
   Maybe<ElementsByName> CreateUIFromJSONData(nlohmann::json data, UIRoot* pRoot, UIElement* pParent);
   Maybe<ElementsByName> CreateUIFromJSONFile(const std::string& filename, UIRoot* pRoot, UIElement* pParent);

private:
   UIConstraint::Target ConstraintTargetFromString(std::string name);
   Maybe<void> ParseUIElement(nlohmann::json element, UIRoot* pRoot, UIElement* pParent, ElementsByName* elementMapOut);
   Maybe<void> ParseConstraints(nlohmann::json constraints, UIRoot* pRoot, const ElementsByName &elementsMap);
};
   
} // CubeWorld
   
} // Engine
