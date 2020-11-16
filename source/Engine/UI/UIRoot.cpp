// By Thomas Steinke

#include <utility>
#include <sstream>

#include <RGBText/Format.h>
#include <RGBLogger/Logger.h>
#include <Engine/Core/Window.h>
#include <Engine/Graphics/Program.h>
#include <Shared/Helpers/TimSort.h>
#include <Shared/UI/RectFilled.h>
#include <Shared/UI/Text.h>

#include "UIRoot.h"

namespace CubeWorld
{

namespace Engine
{

using UI::RectFilled;
using UI::Text;

UIRoot::UIRoot(Input* input)
#pragma warning(disable : 4355) // 'this': used in base member initializer list
    : UIElement(this, nullptr, "Root")
#pragma warning(default : 4355)
   , mInput(input)
   , mActivelyCapturingElement(nullptr)
   , mDirty(false)
{
}

UIRoot::~UIRoot()
{}

}; // namespace Engine

}; // namespace CubeWorld
