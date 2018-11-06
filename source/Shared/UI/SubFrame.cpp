// By Thomas Steinke

#include <glm/glm.hpp>

#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>
#include <Engine/UI/UIRoot.h>

#include "SubFrame.h"

namespace CubeWorld
{

namespace UI
{

void SubFrameUI::Receive(const Engine::ElementAddedEvent& evt)
{
   UIRoot::Receive(evt);
   
   // Special situation: we always wanna know how big the UI is, since it can overflow.
   mSolver.add_constraints({
      mFrame.left <= evt.element->GetFrame().left,
      mFrame.right >= evt.element->GetFrame().right,
      mFrame.top >= evt.element->GetFrame().top,
      mFrame.bottom <= evt.element->GetFrame().bottom,
   });
}

void SubFrameUI::Receive(const Engine::ElementRemovedEvent& evt)
{
   UIRoot::Receive(evt);
   
   // TODO eh?
}

SubFrame::SubFrame(Engine::UIRoot* root, UIElement* parent)
   : UIElement(root, parent)
   , mFramebuffer(Engine::Window::Instance()->GetWidth(), Engine::Window::Instance()->GetHeight())
   , mRegion(root->Reserve<Aggregator::Image>(2))
{
   root->GetAggregator<Aggregator::Image>()->ConnectToTexture(mRegion, mFramebuffer.GetTexture());
}

void SubFrame::Update(TIMEDELTA dt)
{
   glm::tvec2<double> scrolled = Engine::Window::Instance()->GetInput()->GetMouseScroll();

   // Draw elements
   mFramebuffer.Bind();
   mUIRoot.UpdateRoot();
   mUIRoot.Update(dt);
   mUIRoot.RenderRoot();
   mFramebuffer.Unbind();
}

void SubFrame::Redraw()
{
   // Resize the UI root within.
   mUIRoot.SetBounds(mFrame);

   std::vector<Aggregator::ImageData> vertices{
      { mFrame.GetBottomLeft(), glm::vec2(0, 0) },
      { mFrame.GetTopRight(), glm::vec2(1, 1) },
   };

   mRegion.Set(vertices.data());
}

}; // namespace UI

}; // namespace CubeWorld
