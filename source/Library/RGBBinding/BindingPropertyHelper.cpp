// By Thomas Steinke

#include "BindingPropertyHelper.h"

namespace CubeWorld
{

namespace BindingPropertyHelper
{

BindingProperty Difference(const BindingProperty& from, const BindingProperty& to)
{
   if (from == to)
   {
      return BindingProperty::Null;
   }

   if (to.IsObject() && from.IsObject())
   {
      BindingProperty result;
      for (const auto&[key, value] : to.pairs())
      {
         BindingProperty diff = Difference(from[key], value);
         if (!diff.IsNull())
         {
            result[key] = std::move(diff);
         }
      }

      return result;
   }
   
   return to;
}

}; // namespace BindingPropertyHelper

}; // namespace CubeWorld
