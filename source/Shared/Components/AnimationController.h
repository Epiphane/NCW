// By Thomas Steinke

#pragma once

#include <unordered_map>

namespace CubeWorld
{

//
// AnimationControllerBase has the common parts in an AnimationController, like
// transition parameters.
//
struct AnimationControllerBase {
   float GetFloatParameter(const std::string& name) { return floatParams[name]; }
   void SetParameter(const std::string& name, float val) { floatParams[name] = val; }
   
   bool GetBoolParameter(const std::string& name) { return boolParams[name]; }
   void SetBoolParameter(const std::string& name, bool val) { boolParams[name] = val; }

   // Animation FSM parameters
   std::unordered_map<std::string, float> floatParams;
   std::unordered_map<std::string, bool> boolParams;
};

}; // namespace CubeWorld
