// By Thomas Steinke

#include <cassert>

#include <Engine/Core/Paths.h>
#include <Engine/Logger/Logger.h>

#include "StationaryScrubber.h"

namespace CubeWorld
{

namespace Editor
{

StationaryScrubber::StationaryScrubber(
   Bounded& parent,
   const Options& options
)
   : Scrubber(parent, options)
{
   mMin = std::numeric_limits<double>::lowest();
   mMax = std::numeric_limits<double>::max();
   mValue = 0;
}

void StationaryScrubber::MouseDrag(int button, double x, double y)
{
   if (mIsPressed)
   {
      double last = mValue;
      SetValue((x - mOptions.x) / mOptions.w);
      if (mMoveCallback)
      {
         mMoveCallback(mValue - last);
      }
   }
}

void StationaryScrubber::SetValue(double value)
{
   Scrubber::SetValue(value);
   mOffset = glm::vec3(0);
}

}; // namespace Editor

}; // namespace CubeWorld
