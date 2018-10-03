// By Thomas Steinke

#include <cassert>

#include <Engine/Graphics/Program.h>
#include <Engine/Logger/Logger.h>

#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include "Controls.h"
#include "../UI/TextButton.h"
#include "../UI/Image.h"

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

   {
      Image::Options imageOptions;
      imageOptions.z = 0.5f;
      imageOptions.filename = Asset::Image("EditorSidebar.png");
      Add<Image>(imageOptions);
   }

   TextButton::Options labelOptions;
   labelOptions.x = 8.0f / GetWidth();
   labelOptions.y = 1.0f - 43.0f / GetHeight();
   labelOptions.w = 1.0f - 16.0f / GetWidth();
   labelOptions.h = 35.0f / GetHeight();
   labelOptions.text = "N/A";
   labelOptions.onClick = nullptr;
   for (size_t i = 0; i < mLayout.elements.size(); ++i)
   {
      labelOptions.text = mLayout.elements[i].label;
      labelOptions.onClick = mLayout.elements[i].callback;
      Add<TextButton>(labelOptions);
      labelOptions.y -= 35.0f / GetHeight();
   }
}

}; // namespace Editor

}; // namespace CubeWorld
