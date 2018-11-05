// By Thomas Steinke

#include <fstream>
#include <Engine/Core/File.h>
#include <Engine/Core/Window.h>
#include <Shared/Helpers/Asset.h>

#include <Shared/UI/Image.h>
#include <Shared/UI/TextButton.h>
#include <Shared/UI/RectFilled.h>

#include "Sidebar.h"

namespace CubeWorld
{

namespace Editor
{

namespace ModelMaker
{

using Engine::UIElement;
using Engine::UIFrame;
using UI::Image;
using UI::TextButton;
using UI::RectFilled;

Sidebar::Sidebar(Engine::UIRoot* root, UIElement* parent)
   : UIElement(root, parent)
   , mFilename(Paths::Normalize(Asset::Model("dummy.cub")))
{
   {
      RectFilled* bg = Add<RectFilled>(glm::vec4(0.2, 0.2, 0.2, 1));
      RectFilled* fg = Add<RectFilled>(glm::vec4(0, 0, 0, 1));

      UIFrame& fBackground = bg->GetFrame();
      UIFrame& fForeground = fg->GetFrame();
      root->AddConstraints({
         fBackground.left == mFrame.left,
         fBackground.right == mFrame.right,
         fBackground.top == mFrame.top,
         fBackground.bottom == mFrame.bottom,
         mFrame > fForeground,

         fForeground.left == fBackground.left,
         fForeground.right == fBackground.right - 2,
         fForeground.top == fBackground.top - 2,
         fForeground.bottom == fBackground.bottom + 2,
         fForeground > fBackground,
      });
   }

   // Labels
   TextButton::Options buttonOptions;
   buttonOptions.text = "Load";
   buttonOptions.onClick = std::bind(&Sidebar::LoadNewFile, this);
   TextButton* load = Add<TextButton>(buttonOptions);

   buttonOptions.text = "Save";
   buttonOptions.onClick = std::bind(&Sidebar::SaveFile, this);
   mSave = Add<TextButton>(buttonOptions);
   
   buttonOptions.text = "Save As...";
   buttonOptions.onClick = std::bind(&Sidebar::SaveNewFile, this);
   TextButton* saveAs = Add<TextButton>(buttonOptions);
   
   buttonOptions.text = "Discard Changes";
   buttonOptions.onClick = std::bind(&Sidebar::DiscardChanges, this);
   TextButton* discard = Add<TextButton>(buttonOptions);

   UIFrame& fLoad = load->GetFrame();
   UIFrame& fSave = mSave->GetFrame();
   UIFrame& fSaveAs = saveAs->GetFrame();
   UIFrame& fDiscard = discard->GetFrame();
   root->AddConstraints({
      fLoad.left == mFrame.left + 8,
      fLoad.right == mFrame.right - 8,
      fLoad.top == mFrame.top - 8,
      fLoad.height == 32,

      fSave.left == fLoad.left,
      fSave.right == fLoad.right,
      fSave.top == fLoad.bottom - 8,
      fSave.height == fLoad.height,

      fSaveAs.left == fSave.left,
      fSaveAs.right == fSave.right,
      fSaveAs.top == fSave.bottom - 8,
      fSaveAs.height == fSave.height,

      fDiscard.left == fSaveAs.left,
      fDiscard.right == fSaveAs.right,
      fDiscard.top == fSaveAs.bottom - 8,
      fDiscard.height == fSaveAs.height,
   });

   // Create a scrollable list of available models
   UIFrame& fExplorer = Add<UIElement>()->GetFrame();
   root->AddConstraints({
      fExplorer.left == mFrame.left,
      fExplorer.right == mFrame.right,
      fExplorer.bottom == mFrame.bottom,
      fExplorer.top == fDiscard.bottom - 8,
   });

   std::vector<std::string> testData = {"dummy.cub", "aim.cub", "barrel.cub", "bed.cub"};
   TextButton* prevButton = nullptr;
   for (const std::string& file : testData)
   {
      buttonOptions.text = file;
      buttonOptions.onClick = std::bind(&Sidebar::LoadFile, this, Asset::Model(file));
      TextButton* button = Add<TextButton>(buttonOptions);
      UIFrame& frame = button->GetFrame();
      root->AddConstraints({
         frame.left == fExplorer.left,
         frame.right == fExplorer.right,
         frame.height == 32,
      });

      if (prevButton == nullptr)
      {
         root->AddConstraints({ frame.top == fExplorer.top });
      }
      else
      {
         root->AddConstraints({ frame.top == prevButton->GetFrame().bottom });
      }
      prevButton = button;
   }

   root->Subscribe<Engine::ComponentAddedEvent<CubeModel>>(*this);
   root->Subscribe<ModelModifiedEvent>(*this);
}

void Sidebar::Receive(const Engine::ComponentAddedEvent<CubeModel>& evt)
{
   mModel = evt.component;

   LoadFile(mFilename);
}

void Sidebar::Receive(const ModelModifiedEvent&)
{
   SetModified(true);
}

void Sidebar::SetModified(bool modified)
{
   if (mModified == modified)
   {
      // Unchanged.
      return;
   }

   mModified = modified;

   std::string title = "NCW - Model Maker - ";
   if (mModified)
   {
      title += "*";
   }
   title += Paths::GetFilename(mFilename);
   Engine::Window::Instance()->SetTitle(title);

   mSave->SetText(modified ? "*Save" : "Save");
}

void Sidebar::LoadNewFile()
{
   std::string file = OpenFileDialog(mFilename, {});
   if (!file.empty())
   {
      mFilename = file;
      LoadFile(file);
   }
}

void Sidebar::LoadFile(const std::string& filename)
{
   if (!mModel)
   {
      // Wait until the component exists!
      return;
   }

   // Load
   mModel->Load(filename);
   mpRoot->Emit<ModelLoadedEvent>(mModel);
   SetModified(false);
}

void Sidebar::SaveNewFile()
{
   std::string file = SaveFileDialog(mFilename);
   if (!file.empty())
   {
      mFilename = file;
      SaveFile();
   }
}

void Sidebar::SaveFile()
{
   // TODO
   return;
   //std::string serialized = mModel->Serialize();
   //std::ofstream out(mFilename);
   //out << serialized << std::endl;

   mpRoot->Emit<ModelSavedEvent>(mModel);
   SetModified(false);
}

void Sidebar::DiscardChanges()
{
   LoadFile(mFilename);
}

}; // namespace AnimationStation

}; // namespace Editor

}; // namespace CubeWorld
