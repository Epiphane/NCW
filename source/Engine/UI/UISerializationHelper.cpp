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
#include "UIStackView.h"

#include "UISerializationHelper.h"

namespace CubeWorld
{

namespace Engine
{
   
static const std::map<UIConstraint::Target, std::string> CONSTRAINT_NAME_MAPPING = {
   {UIConstraint::Left,     "Left"},
   {UIConstraint::Bottom,   "Bottom"},
   {UIConstraint::Top,      "Top"},
   {UIConstraint::Right,    "Right"},
   {UIConstraint::Width,    "Width"},
   {UIConstraint::Height,   "Height"},
   {UIConstraint::CenterX,  "CenterX"},
   {UIConstraint::CenterY,  "CenterY"},
   {UIConstraint::ZHeight,  "ZHeight"},
   {UIConstraint::NoTarget, "NoTarget"},
};

#pragma mark Going from JSON to UI

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
 *
 * @param element        JSON data representing the element tree to parse
 * @param pRoot          Root element all the elements will belong to
 * @param pParent        Element that the hierarchy will be parented to
 * @param elementMapOut  Pass in an ElementsByName reference that will be populated with the elements that are parsed
 *                          from the JSON.
 */
Maybe<void> UISerializationHelper::ParseUIElement(nlohmann::json element, UIRoot* pRoot, UIElement* pParent, ElementsByName* elementMapOut)
{
   std::unique_ptr<UIElement> newElement;
   std::string newElementClass = element["class"].get<std::string>();

   if (newElementClass.compare("UIElement") == 0) {
      newElement = std::make_unique<UIElement>(pRoot, pParent, element["name"]);
   }
   else if (newElementClass.compare("UIRectFilled") == 0) {
      newElement = std::make_unique<UI::RectFilled>(pRoot, pParent, element["name"]);
   }
   else if (newElementClass.compare("UIStackView") == 0) {
      newElement = std::make_unique<UIStackView>(pRoot, pParent, element["name"]);
   }
   else {
      return Failure{"Unsupported class name in file: %1", newElementClass.c_str()};
   }

   // Let each class do some initialization based on the JSON data
   newElement->InitFromJSON(element);

   if (elementMapOut->find(element["name"]) != elementMapOut->end()) {
      std::string badName = element["name"];
      return Failure{"Duplicate element name %1 in file", badName.c_str()};
   }

   UIElement* reference = pParent->AddChild(std::move(newElement));
   (*elementMapOut)[element["name"]] = reference;

   for (auto child : element["children"]) {
      Maybe<void> result = ParseUIElement(child, pRoot, reference, elementMapOut);
      
      if (!result) {
         return result; // Return on Failure state
      }
   }
   
   return Success;
}

UIConstraint::Target UISerializationHelper::ConstraintTargetFromString(std::string name)
{
   for (auto const& [target, mapName] : CONSTRAINT_NAME_MAPPING) {
      if (name == mapName) {
         return target;
      }
   }
   
   assert(false && "Unknown constraint target name");
   return UIConstraint::NoTarget;
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

 * @param pRoot         UIRoot that will be receiving these constraints.
 * @param elementsMap   An elementsMap of all the elements that are involved in constraints
 */
Maybe<void> UISerializationHelper::ParseConstraints(nlohmann::json constraints, UIRoot* pRoot, const ElementsByName &elementsMap)
{
   for (auto constraintData : constraints) {
      std::string primaryElementName   = constraintData["primaryElement"];
      std::string secondaryElementName = constraintData.value("secondaryElement", "");

      std::string primaryTargetName   = constraintData["primaryTarget"];
      std::string secondaryTargetName = constraintData.value("secondaryTarget", "NoTarget");

      UIElement* primaryElement = nullptr;
      UIElement* secondaryElement = nullptr;

      if (elementsMap.find(primaryElementName) == elementsMap.end()) {
         return Failure{"Unknown element name %1 when creating constraints.", primaryElementName};
      }
      
      primaryElement = elementsMap.at(primaryElementName);

      if (!secondaryElementName.empty()) {
         if (elementsMap.find(secondaryElementName) == elementsMap.end()) {
            return Failure{"Unknown element name %1 when creating constraints.", secondaryElementName};
         }
         secondaryElement = elementsMap.at(secondaryElementName);
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

   return Success;
}

/**
 * Create a new UIElement hierarchy based on data from the specified JSON data,
 *  and adds it as a child to the specified parent.
 *
 * Also adds constraints from the JSON data to the specified UIRoot.
 *
 * Returns a map of newly created elements by their name, or a Failure state
 *  if we have bad data (unknown class names, etc.)
 */
Maybe<ElementsByName> UISerializationHelper::CreateUIFromJSONData(nlohmann::json data, UIRoot* pRoot, UIElement* pParent)
{
   ElementsByName elementMap; // Passed into ParseUIElement and populated there
   Maybe<void> parsingResult;

   parsingResult = ParseUIElement(data["baseElement"], pRoot, pParent, &elementMap);
   if (!parsingResult) {
      return parsingResult.Failure();
   }

   parsingResult = ParseConstraints(data["constraints"], pRoot, elementMap);
   if (!parsingResult) {
      return parsingResult.Failure();
   }

   return elementMap;
}
   
//
// Helper function that parses the JSON at the given path then calls CreateUIFromJSONData.
//
// Returns a failure state if the file is missing or invalid JSON.
//
Maybe<ElementsByName> UISerializationHelper::CreateUIFromJSONFile(const std::string& filename, UIRoot* pRoot, UIElement* pParent)
{
   Maybe<nlohmann::json> data = Shared::GetJsonFromFile(filename);
   
   if (!data) {
      return Failure(data.Failure()).WithContext("UI Creation Failed");
   }
   
   return CreateUIFromJSONData(*data, pRoot, pParent);
}
   
#pragma mark Going from UI to JSON

//
// Serialize a UIElement's heirarchy to JSON, including the constraints passed in from the editor.
//
nlohmann::json UISerializationHelper::CreateJSONFromUI(UIElement *element, const std::vector<UIConstraint>& constraints) {
   nlohmann::json uiElementData;
   
   SerializeUIElement(element, &uiElementData);
   nlohmann::json constraintData = SerializeConstraints(element, constraints);
   
   nlohmann::json result;
   result["baseElement"] = uiElementData;
   result["constraints"] = constraintData;
   
   return result;
}
   
//
// Converts the specified UIElement to JSON. Recursively adds the data to dataIn by
//    going through the children of 'element'.
//
void UISerializationHelper::SerializeUIElement(UIElement* element, nlohmann::json* dataIn) 
{
   
}
   
std::string UISerializationHelper::StringFromConstraintTarget(UIConstraint::Target target) 
{
   if (CONSTRAINT_NAME_MAPPING.find(target) == CONSTRAINT_NAME_MAPPING.end()) {
      assert(false && "Unknown constraint target name");
      return "NoTarget";
   }
   
   return CONSTRAINT_NAME_MAPPING.at(target);
}

//
// Looks at the given UIElement's UIRoot and grabs all its constraints. Then, we filter
//    to get only the constraints involving the UIElement and its descendants. Then,
//    convert them all to JSON.
//
nlohmann::json UISerializationHelper::SerializeConstraints(UIElement* element, const std::vector<UIConstraint>& constraints) 
{
   nlohmann::json result;
   
   for (UIConstraint constraint : constraints) {
      nlohmann::json constraintJson;
      constraintJson["primaryElement"] = constraint.GetPrimaryElement()->GetName();
      constraintJson["primaryTarget"] = StringFromConstraintTarget(constraint.GetPrimaryTarget());
      
      if (constraint.GetSecondaryElement()) {
         constraintJson["secondaryElement"] = constraint.GetSecondaryElement()->GetName();
         constraintJson["secondaryTarget"] = StringFromConstraintTarget(constraint.GetSecondaryTarget());
      }
      
      const UIConstraint::Options& opts = constraint.GetOptions();
      constraintJson["constant"] = opts.constant;
      constraintJson["multiplier"] = opts.multiplier;
      
      result.push_back(constraintJson);
   }
   
   return result;
}

} // CubeWorld

} // Engine

