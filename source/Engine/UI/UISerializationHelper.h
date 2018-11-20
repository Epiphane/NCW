//
//  UISerializationHelper.h
//  Engine
//
//  Created by Elliot Fiske on 11/19/18.
//

#pragma once

#include <string>

namespace CubeWorld
{
   
namespace Engine
{

class UIElement; // Forward declare
   
class UISerializationHelper {
public:
   UIElement* CreateUIFromJSONFile(std::string filename, UIRoot* pRoot, UIElement* pParent);

private:
   UIElement* ParseUIElement(nlohmann::json element, UIRoot* pRoot, UIElement* pParent);
};
   
} // CubeWorld
   
} // Engine
      


