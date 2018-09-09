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

#include "../Helpers/Controls.h"
#include "../Helpers/SubWindow.h"

namespace CubeWorld
{

namespace Editor
{

class AnimationStation : public Engine::State, public Engine::Receiver<AnimationStation> {
public:
   AnimationStation(Engine::Window* window, Bounded& parent, Controls* controls);
   ~AnimationStation();

   void Start() override;
   void Update(TIMEDELTA dt) override;

   void Receive(const NamedEvent& namedEvent);
   void Receive(const MouseDragEvent& evt);
   void Receive(const MouseDownEvent& evt);
   void Receive(const MouseUpEvent& evt);
   void Receive(const MouseClickEvent& evt);

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
   Engine::Graphics::CameraHandle mCamera;

   Engine::Window* mWindow;
   Bounded& mParent;

   Controls* mControls;

   // Layout elements
   Controls::Layout mLayout;
   Controls::Layout::Element& mLoad;
   Controls::Layout::Element& mSave;
   Controls::Layout::Element& mSaveAs;
   Controls::Layout::Element& mDiscard;
   Controls::Layout::Element& mQuit;

   SubWindow mPreview;
   SubWindow mDock;

   Engine::Entity mPlayer;
   std::string mFilename;
};

}; // namespace Editor

}; // namespace CubeWorld
