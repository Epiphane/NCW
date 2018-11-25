// By Thomas Steinke

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <Engine/UI/UIElement.h>
#include <Shared/UI/RectFilled.h>

namespace CubeWorld
{

namespace Editor
{

class Controls : public UI::RectFilled {
public:
   using Option = std::pair<std::string, std::function<void()>>;
   using Options = std::vector<Option>;

public:
   Controls(Engine::UIRoot* root, UIElement* parent, const Options& options);

private:
   // Actions

private:
   // Elements

public:
   // Event handlers

private:
   // State
};

}; // namespace Editor

}; // namespace CubeWorld
