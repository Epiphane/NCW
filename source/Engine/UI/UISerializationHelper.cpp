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
#include "UIRoot.h"

#include "UISerializationHelper.h"

namespace CubeWorld
{

namespace Engine
{


/**
 * Parse a single UIElement and its children.
 *
 * Constructs a std::map where the value is the element's name and the
 *  value is a pointer to the element.
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
void UISerializationHelper::ParseUIElement(nlohmann::json element, UIRoot* pRoot, UIElement* pParent, ElementsByName& elementMapOut)
{
   UIElement* newElement;

   if (element["class"].get<std::string>().compare("UIElement") == 0) {
      newElement = new UIElement(pRoot, pParent, element["name"]);
   }
   else if (element["class"].get<std::string>().compare("UIRectFilled") == 0) {
      newElement = new UI::RectFilled(pRoot, pParent, element["name"]);
   }
   else {
      assert(false && "Unsupported class name! I should probably add it >_>");
   }

   // Let each class do some initialization based on the JSON data
   newElement->InitFromJSON(element);

   if (elementMapOut.find(element["name"]) != elementMapOut.end()) {
      assert(false && "You duplicated a UIElement name! Don't do that!");
   }

   elementMapOut[element["name"]] = newElement;

   std::unique_ptr<UIElement> uniqueElement(newElement);
   pParent->AddChild(std::move(uniqueElement));

   for (auto child : element["children"]) {
      ParseUIElement(child, pRoot, newElement, elementMapOut);
   }
}

UIConstraint::Target UISerializationHelper::ConstraintTargetFromString(std::string name)
{
   static std::map<std::string, UIConstraint::Target> mapping = {
         {"Left",     UIConstraint::Left},
         {"Bottom",   UIConstraint::Bottom},
         {"Top",      UIConstraint::Top},
         {"Right",    UIConstraint::Right},
         {"Width",    UIConstraint::Width},
         {"Height",   UIConstraint::Height},
         {"CenterX",  UIConstraint::CenterX},
         {"CenterY",  UIConstraint::CenterY},
         {"ZHeight",  UIConstraint::ZHeight},
         {"NoTarget", UIConstraint::NoTarget},
   };

   if (mapping.find(name) == mapping.end()) {
      assert(false && "Unknown constraint target name");
      return UIConstraint::NoTarget;
   }

   return mapping[name];
}

/**
 * Parse constraints from JSON data.
 *
 * @param constraints Constraint data. Example structure:

 [
		{
			"primaryElement" : "TestRedRectangle",
			"secondaryElement" : "TestBlueRectangle",
			"primaryTarget" : "Right",
			"secondaryTarget" : "Left",
			"constant" : 8.0
		},
		{
			"primaryElement" : "TestRedRectangle",
			"secondaryElement" : "TestJSONStuff",
			"primaryTarget" : "Width",
			"secondaryTarget" : "Width",
			"multiplier" : 0.5
		}
]

 * @param pRoot UIRoot that will be receiving these constraints.
 */
void UISerializationHelper::ParseConstraints(nlohmann::json constraints, UIRoot* pRoot, ElementsByName &elementsMap)
{
   for (auto constraintData : constraints) {
      std::string primaryElementName   = constraintData["primaryElement"];
      std::string secondaryElementName = constraintData.value("secondaryElement", "");

      std::string primaryTargetName   = constraintData["primaryTarget"];
      std::string secondaryTargetName = constraintData.value("secondaryTarget", "NoTarget");

      UIElement* primaryElement = nullptr;
      UIElement* secondaryElement = nullptr;

      assert(elementsMap.find(primaryElementName) != elementsMap.end() && "Unknown element name in constraint JSON");
      primaryElement = elementsMap[primaryElementName];

      if (!secondaryElementName.empty()) {
         assert(elementsMap.find(secondaryElementName) != elementsMap.end() && "Unknown element name in constraint JSON");
         secondaryElement = elementsMap[secondaryElementName];
      }

      UIConstraint::Target primaryTarget = ConstraintTargetFromString(primaryTargetName);
      UIConstraint::Target secondaryTarget = ConstraintTargetFromString(secondaryTargetName);

      UIConstraint::Options options;
      options.customNameConnector = constraintData.value("name", "");
      options.constant = constraintData.value("constant", 0.0);
      options.multiplier = constraintData.value("multiplier", 1.0);
      options.priority = constraintData.value("mPriority", UIConstraint::REQUIRED_PRIORITY);

      UIConstraint newConstraint(primaryElement, secondaryElement, primaryTarget, secondaryTarget, options);

      pRoot->AddConstraint(newConstraint);
   }
}

/**
 * Create a new UIElement hierarchy based on data from the specified JSON file,
 *  and adds it as a child to the specified parent.
 *
 * Also adds constraints from the JSON file to the specified UIRoot.
 */
UIElement* UISerializationHelper::CreateUIFromJSONFile(const std::string &filename, UIRoot* pRoot, UIElement* pParent)
{
   std::ifstream file(filename);

   if (!file.good()) {
      assert(false && "Something is terribly wrong with that file!");
   }

   nlohmann::json data;
   file >> data;

   ElementsByName elementsByName;

   ParseUIElement(data["baseElement"], pRoot, pParent, elementsByName);

   ParseConstraints(data["constraints"], pRoot, elementsByName);

   return elementsByName[data["baseElement"]["name"]];
}


} // CubeWorld

} // Engine

