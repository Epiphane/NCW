//
//  UISerializationHelper.cpp
//  Engine
//
//  Created by Elliot Fiske on 11/19/18.
//

#include <RGBNetworking/JSONSerializer.h>
#include <Shared/UI/RectFilled.h>

#include "UIElement.h"
#include "UIRoot.h"
#include "UIStackView.h"

#include "UISerializationHelper.h"

namespace CubeWorld
{

namespace Engine
{

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
Maybe<void> UISerializationHelper::ParseUIElement(const BindingProperty& element, UIRoot* pRoot, UIElement* pParent, ElementsByName* elementMapOut)
{
   std::unique_ptr<UIElement> newElement;
   std::string newElementClass = element["class"];

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
      return Failure{"Unsupported class name in file: {name}", newElementClass};
   }

   // Let each class do some initialization based on the JSON data
   newElement->InitFromJSON(element);

   if (elementMapOut->find(element["name"]) != elementMapOut->end()) {
      return Failure{"Duplicate element name {name} in file", element["name"].GetStringValue()};
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
Maybe<void> UISerializationHelper::ParseConstraints(const BindingProperty& constraints, UIRoot* pRoot, const ElementsByName &elementsMap)
{
   for (const auto& constraintData : constraints) {
      std::string primaryElementName   = constraintData["primaryElement"];
      std::string secondaryElementName = constraintData["secondaryElement"];

      std::string primaryTargetName   = constraintData["primaryTarget"];
      std::string secondaryTargetName = constraintData["secondaryTarget"];

      UIElement* primaryElement = nullptr;
      UIElement* secondaryElement = nullptr;

      if (elementsMap.find(primaryElementName) == elementsMap.end()) {
         return Failure{"Unknown element name {name} when creating constraints.", primaryElementName};
      }

      primaryElement = elementsMap.at(primaryElementName);

      if (!secondaryElementName.empty()) {
         if (elementsMap.find(secondaryElementName) == elementsMap.end()) {
            return Failure{"Unknown element name {name} when creating constraints.", secondaryElementName};
         }
         secondaryElement = elementsMap.at(secondaryElementName);
      }

      UIConstraint::Target primaryTarget = UIConstraint::ConstraintTargetFromString(primaryTargetName);
      UIConstraint::Target secondaryTarget = UIConstraint::ConstraintTargetFromString(secondaryTargetName);

      UIConstraint::Options options;
      options.customNameConnector = constraintData["name"];
      options.constant = constraintData["constant"].GetDoubleValue();
      options.multiplier = constraintData["multiplier"].GetDoubleValue(1.0);
      options.priority = constraintData["mPriority"].GetDoubleValue(UIConstraint::REQUIRED_PRIORITY);

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
Maybe<ElementsByName> UISerializationHelper::CreateUIFromJSONData(const BindingProperty& data, UIRoot* pRoot, UIElement* pParent)
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
   Maybe<BindingProperty> data = JSONSerializer::DeserializeFile(filename);

   if (!data) {
      return Failure(data.Failure()).WithContext("UI Creation Failed");
   }

   return CreateUIFromJSONData(*data, pRoot, pParent);
}

#pragma mark Going from UI to JSON

//
// Serialize a UIElement's heirarchy to JSON, including the constraints passed in from the editor.
//
BindingProperty UISerializationHelper::CreateJSONFromUI(UIElement *element, const std::vector<UIConstraint>& constraints) {
   BindingProperty result;

   result["baseElement"] = element->ConvertToJSON();
   result["constraints"] = SerializeConstraints(constraints);

   return result;
}

//
// Given the list of constraints from the Editor, serialize everything to a JSON file.
//    Builds the constraint JSON data in the provided json parameter.
//
BindingProperty UISerializationHelper::SerializeConstraints(const std::vector<UIConstraint>& constraints)
{
   BindingProperty result;
   for (UIConstraint constraint : constraints) {
      BindingProperty constraintJson = result.push_back(BindingProperty{});
      constraintJson["primaryElement"] = constraint.GetPrimaryElement()->GetName();
      constraintJson["primaryTarget"] = UIConstraint::StringFromConstraintTarget(constraint.GetPrimaryTarget());

      if (constraint.GetSecondaryElement()) {
         constraintJson["secondaryElement"] = constraint.GetSecondaryElement()->GetName();
         constraintJson["secondaryTarget"] = UIConstraint::StringFromConstraintTarget(constraint.GetSecondaryTarget());
      }

      const UIConstraint::Options& opts = constraint.GetOptions();
      constraintJson["constant"] = opts.constant;
      constraintJson["multiplier"] = opts.multiplier;
   }
   return result;
}

} // CubeWorld

} // Engine
