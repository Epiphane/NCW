// By Thomas Steinke

#include <Shared/Helpers/Asset.h>

#include "../UI/Image.h"
#include "../UI/Label.h"

#include "Dock.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

Dock::Dock(
   Bounded& parent,
   const Options& options,
   MainState* state
)
   : SubWindow(parent, options)
   , mState(state)
{
   Image::Options imageOptions;
   imageOptions.x = 8.0f / GetWidth();
   imageOptions.y = 10.0f / GetHeight();
   imageOptions.w = 40.0f / GetWidth();
   imageOptions.h = 40.0f / GetHeight();
   imageOptions.filename = Asset::Image("EditorIcons.png");
   imageOptions.image = "button_play";
   imageOptions.hoverImage = "hover_button_play";
   imageOptions.pressImage = "press_button_play";
   Add<Image>(imageOptions);
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld
