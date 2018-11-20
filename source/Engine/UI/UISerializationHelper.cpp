//
//  UISerializationHelper.cpp
//  Engine
//
//  Created by Elliot Fiske on 11/19/18.
//

#include <fstream>
#include <Shared/Helpers/json.hpp>
#include <Shared/UI/RectFilled.h>

#include "UIElement.h"

#include "UISerializationHelper.h"

namespace CubeWorld
{

namespace Engine
{


/**
 * Parse a single UIElement and its children.
 *
 * Example JSON structure:
 *
 * {
		"class" : "UIElement",
		"name" : "ExampleElement",
		"children" : [
			{
				"class" : "UIRectFilled",
				"name" : "TestRedRectangle",
				"backgroundColor" : [1.0, 0, 0, 1.0]
			},
			{
				"class" : "UIRectFilled",
				"name" : "TestBlueRectangle",
				"backgroundColor" : [0, 0, 1.0, 1.0]
			},
		]
	}
 */
UIElement* UISerializationHelper::ParseUIElement(nlohmann::json element, UIRoot* pRoot, UIElement* pParent)
{
   UIElement* result;

   if (element["class"] == "UIElement") {
      result = new UIElement(pRoot, pParent, element["name"]);
   }
   else if (element["class"] == "UIRectFilled") {
      result = new UI::RectFilled(pRoot, pParent, element["name"]);
   }


   for (auto& child : element["children"]) {

   }

   return result;
}


/**
 * Create a new UIElement hierarchy based on data from the specified JSON file.
 *  Also returns the constraints between them.
 *
 * NOTE: It's up to the caller to free() the memory this class
 *          creates.
 */
UIElement* UISerializationHelper::CreateUIFromJSONFile(std::string filename, UIRoot* pRoot, UIElement* pParent)
{
   std::ifstream file(filename);
   nlohmann::json data;
   file >> data;

   UIElement* result = ParseUIElement(data["baseElement"], pRoot, pParent);

   return result;
}

} // CubeWorld

} // Engine

