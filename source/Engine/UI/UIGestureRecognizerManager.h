//
// UIGestureRecognizerManager.h
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <vector>

#include <Engine/Core/Singleton.h>

#include "UIGestureRecognizer.h"

namespace CubeWorld
{

namespace Engine
{

class UIGestureRecognizerManager : Engine::Singleton<UIGestureRecognizerManager>
{
public:

private:
   std::vector<std::weak_ptr<UIGestureRecognizer>> mRecognizers;

};

} // namespace Engine

} // namespace CubeWorld

