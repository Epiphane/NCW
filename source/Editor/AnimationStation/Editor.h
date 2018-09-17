// By Thomas Steinke

#pragma once

#include <memory>

#include "../UI/StateWindow.h"
#include "../UI/SubWindow.h"
#include "Dock.h"
#include "Sidebar.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

class Editor : public SubWindow
{
public:
   Editor(
      Bounded& parent,
      const Options& options
   );

   //
   // Called every time this editor is reactivated.
   //
   void Start();

private:
   Sidebar* mSidebar;
   Dock* mDock;
   StateWindow* mStateWindow;
   std::unique_ptr<MainState> mState;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld
