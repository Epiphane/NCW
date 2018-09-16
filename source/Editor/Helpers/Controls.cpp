// By Thomas Steinke

#include <cassert>

#include <Engine/Graphics/Program.h>
#include <Engine/Logger/Logger.h>

#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include "Controls.h"
#include "../UI/Label.h"

namespace CubeWorld
{

namespace Editor
{

Controls::Controls(
   Bounded& parent,
   const Options& options
)
   : SubWindow(parent, options)
   , mLayout{{}}
{
}

void Controls::SetLayout(const Layout& layout)
{
   mLayout = layout;

   Rebuild();
}

void Controls::Rebuild()
{
   mChildren.clear();

   Element::Options elementOptions;
   elementOptions.x = 8.0f / GetWidth();
   elementOptions.y = 1.0f - 43.0f / GetHeight();
   elementOptions.w = 1.0f - 16.0f / GetWidth();
   elementOptions.h = 35.0f / GetHeight();
   Label::Options labelOptions;
   labelOptions.text = "N/A";
   labelOptions.onClick = nullptr;
   for (size_t i = 0; i < mLayout.elements.size(); ++i)
   {
      labelOptions.text = mLayout.elements[i].label;
      labelOptions.onClick = mLayout.elements[i].callback;
      Add<Label>(elementOptions, labelOptions);
      elementOptions.y -= 35.0f / GetHeight();
   }
}

}; // namespace Editor

}; // namespace CubeWorld
