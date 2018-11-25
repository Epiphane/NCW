// By Thomas Steinke

#include <Engine/Core/File.h>
#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>
#include <Engine/UI/UIStackView.h>

#include <Shared/UI/TextButton.h>

#include "Command/Commands.h"
#include "Command/CommandStack.h"
#include "Controls.h"

namespace CubeWorld
{

namespace Editor
{

using Engine::UIElement;
using UI::TextButton;
using UI::RectFilled;

Controls::Controls(Engine::UIRoot* root, UIElement* parent, const std::vector<Option>& options)
   : RectFilled(root, parent, "MainControls", glm::vec4(0.2, 0.2, 0.2, 1))
{
   RectFilled* foreground = Add<RectFilled>("MainControlsFG", glm::vec4(0, 0, 0, 1));

   foreground->ConstrainDimensionsTo(this, -4.0);
   foreground->ConstrainCenterTo(this);

   Engine::UIStackView* buttons = foreground->Add<Engine::UIStackView>("MainControlsSidebarStackView");
   buttons->SetOffset(8.0);

   for (const Option& option : options)
   {
      TextButton::Options buttonOptions;
      buttonOptions.text = option.first;
      buttonOptions.onClick = option.second;
      TextButton* button = buttons->Add<TextButton>(buttonOptions);

      button->ConstrainLeftAlignedTo(buttons);
      button->ConstrainWidthTo(buttons);
      button->ConstrainHeight(32);
   }

   buttons->ConstrainCenterTo(foreground);
   buttons->ConstrainDimensionsTo(foreground, -16);

}

}; // namespace Editor

}; // namespace CubeWorld
