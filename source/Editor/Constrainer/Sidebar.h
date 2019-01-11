// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/UI/UIElement.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/TextButton.h>
#include <Shared/UI/RectFilled.h>

//#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

using UI::TextButton;

class Sidebar : public Engine::UIElement {
public:
   Sidebar(Engine::UIRoot* root, UIElement* parent);

private:
   // Actions
   void SetModified(bool modified);

   void LoadNewFile();
   void SaveNewFile();

   void SaveFile();
   void LoadFile(const std::string& filename);

   void DiscardChanges();
   void Quit();

private:
   // Elements
   TextButton* mSave;
   TextButton* mQuit;

private:
   // State
   std::string mFilename;
   bool mModified;
};

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld
