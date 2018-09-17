// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Bounded.h>
#include <Engine/Core/State.h>
#include <Engine/Core/Window.h>
#include <Engine/Event/InputEvent.h>
#include <Engine/Event/Receiver.h>
#include <Engine/Graphics/Camera.h>
#include <Shared/Event/NamedEvent.h>

#include "../UI/Label.h"
#include "../UI/SubWindow.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace AnimationStation
{

class Sidebar : public SubWindow {
public:
   Sidebar(
      Bounded& parent,
      const Options& options,
      MainState* state
   );

private:
   void SetModified(bool modified);
   bool mModified;

   void LoadNewFile();
   void SaveNewFile();

   void SaveFile();
   void LoadFile(const std::string& filename);

   void DiscardChanges();
   void Quit();

private:
   Label* mLoad;
   Label* mSave;
   Label* mSaveAs;
   Label* mDiscard;
   Label* mQuit;

private:
   MainState* mState;
   std::string mFilename;
};

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld
