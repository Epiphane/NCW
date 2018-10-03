// By Thomas Steinke

#include <Shared/Helpers/Asset.h>

#include "../Command/CommandStack.h"
#include "../UI/Image.h"

#include "Dock.h"

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

Dock::Dock(
   Bounded& parent,
   const Options& options
)
   : SubWindow(parent, options)
{
   const float EIGHT_X = 8.0f / GetWidth();
   const float EIGHT_Y = 8.0f / GetHeight();

   // Background
   {
      Image::Options imageOptions;
      imageOptions.x = 0.0f;
      imageOptions.y = 0.0f;
      imageOptions.z = 0.5f;
      imageOptions.w = 1.0f;
      imageOptions.h = 1.0f;
      imageOptions.filename = Asset::Image("EditorDock.png");
      Add<Image>(imageOptions);
   }

   Subscribe<Engine::ComponentAddedEvent<Game::CubeModel>>(*this);
}

///
///
///
void Dock::Receive(const Engine::ComponentAddedEvent<Game::CubeModel>&)
{
}

///
///
///
void Dock::Update(TIMEDELTA dt)
{
   SubWindow::Update(dt);
}

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld
