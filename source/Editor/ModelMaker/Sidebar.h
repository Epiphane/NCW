// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/UI/UIElement.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/TextButton.h>

#include "Events.h"
#include "State.h"

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
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

public:
   // Event handlers
   void Receive(const Engine::ComponentAddedEvent<CubeModel>& evt);
   void Receive(const ModelModifiedEvent& evt);

private:
   // State
   std::string mFilename;
   Engine::ComponentHandle<CubeModel> mModel;
   bool mModified;
};

}; // namespace ModelMaker

}; // namespace Editor

}; // namespace CubeWorld
