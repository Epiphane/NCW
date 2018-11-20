//
//  UISerializationHelper.h
//  Engine
//
//  Created by Elliot Fiske on 11/19/18.
//

#pragma once

#include <string>
#include <map>

#include <Shared/Helpers/json.hpp>

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
   UIElement* CreateUIFromJSONFile(const std::string &filename, UIRoot* pRoot, UIElement* pParent);

private:
   UIConstraint::Target ConstraintTargetFromString(std::string name);
   void ParseUIElement(nlohmann::json element, UIRoot* pRoot, UIElement* pParent, ElementsByName& elementMap);
   void ParseConstraints(nlohmann::json constraints, UIRoot* pRoot, ElementsByName &elementsMap);
};
   
} // CubeWorld
   
} // Engine