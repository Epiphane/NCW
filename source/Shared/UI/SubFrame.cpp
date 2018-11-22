// By Thomas Steinke

#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>
#include <Engine/UI/UIRoot.h>

#include "SubFrame.h"

namespace CubeWorld
{

namespace UI
{

void SubFrameUIRoot::Receive(const Engine::ElementAddedEvent& evt)
{
   UIRoot::Receive(evt);
   
   // Special situation: we always wanna know how big the UI is, since it can overflow.
   mSolver.add_constraints({
      mLeft <= evt.element->GetFrame().left,
      mRight >= evt.element->GetFrame().right,
      mTop >= evt.element->GetFrame().top,
      mBottom <= evt.element->GetFrame().bottom,
   });
}

void SubFrameUIRoot::Receive(const Engine::ElementRemovedEvent& evt)
{
   UIRoot::Receive(evt);
   
   // TODO eh?
}

SubFrame::SubFrame(Engine::UIRoot* root, UIElement* parent)
   : UIElement(root, parent)
   , mFramebuffer(Engine::Window::Instance()->GetWidth(), Engine::Window::Instance()->GetHeight())
   , mScroll{0, 0}
   , mRegion(root->Reserve<Aggregator::Image>(2))
{
   root->GetAggregator<Aggregator::Image>()->ConnectToTexture(mRegion, mFramebuffer.GetTexture());

   metric = DebugHelper::Instance()->RegisterMetric("Update Sub UI", [&]() -> std::string {
      return Format::FormatString("%.1f", mUpdateTimer.Average());
   });
}

void SubFrame::Update(TIMEDELTA dt)
{
   glm::tvec2<double> scrolled = 10.0 * Engine::Window::Instance()->GetMouseScroll();
   glm::tvec2<double> mouse = Engine::Window::Instance()->GetRawMousePosition();
   if (ContainsPoint(mouse.x, mouse.y))
   {
      if (scrolled.x != 0 || scrolled.y != 0)
      {
         glm::tvec2<double> newScroll{mScroll.x - scrolled.x, mScroll.y - scrolled.y};
         //mScroll.x = static_cast<uint32_t>(std::max(newScroll.x, 0.0));
         mScroll.y = static_cast<uint32_t>(std::max(newScroll.y, 0.0));

         mUIRoot.SetBounds(Bounds{mScroll.x, mScroll.y, mFrame.GetWidth(), mFrame.GetHeight()});
      }
   }

   // Draw elements
   mFramebuffer.Bind();
   mUpdateTimer.Reset();
   mUIRoot.UpdateRoot();
   mUpdateTimer.Elapsed();
   mUIRoot.Update(dt);
   mUIRoot.RenderRoot();
   mFramebuffer.Unbind();
}

void SubFrame::Redraw()
{
   // Resize the UI root within.
   mUIRoot.SetBounds(Bounds{mScroll.x, mScroll.y, mFrame.GetWidth(), mFrame.GetHeight()});

   glm::vec2 textureSize{mFramebuffer.GetWidth(), mFramebuffer.GetHeight()};
   glm::vec2 uvSize = glm::vec2(mFrame.GetWidth(), mFrame.GetHeight()) / textureSize;
   glm::vec2 uvBottomLeft{
      mScroll.x / textureSize.x,
      1 - mScroll.y / textureSize.y - uvSize.y
   };

   LOG_DEBUG("UV: [%1, %2] -> [%3, %4]", uvBottomLeft.x, uvBottomLeft.y, uvBottomLeft.x + uvSize.x, uvBottomLeft.y + uvSize.y);

   uvBottomLeft = glm::vec2(0);
   //uvSize = glm::vec2(0.5);

   std::vector<Aggregator::ImageData> vertices{
      { mFrame.GetBottomLeft(), uvBottomLeft },
      { mFrame.GetTopRight(), uvBottomLeft + uvSize },
   };

   mRegion.Set(vertices.data());
}

void SubFrame::AddConstraints(const rhea::constraint_list& constraints)
{
   mUIRoot.AddConstraints(constraints);
}

}; // namespace UI

}; // namespace CubeWorld
