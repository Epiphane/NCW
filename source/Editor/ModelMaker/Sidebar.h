// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/UI/UIElement.h>
#include <Engine/UI/UIRoot.h>
#include <Shared/UI/RectFilled.h>
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

class Sidebar : public UI::RectFilled {
public:
   Sidebar(Engine::UIRoot* root, Engine::UIElement* parent);

private:
   // Actions
   void SetModified(bool modified);

   void LoadNewFile();
   void SaveNewFile();

   void SaveFile();
   void LoadFile(const std::string& filename);

   void DiscardChanges();

private:
   // Elements
   TextButton* mSave;

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
