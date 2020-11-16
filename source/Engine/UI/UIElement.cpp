//
// UIElement – Basic UI element. Stores its "frame," which gives the element
//                its coordinates and size.
//
// By Thomas Steinke

#include <sstream>

#include <RGBText/Format.h>
#include <RGBText/StringHelper.h>
#include <RGBLogger/Logger.h>

#include "UIElement.h"
#include "UIRoot.h"

namespace CubeWorld
{

namespace Engine
{

UIElement::UIElement(UIRoot* root, UIElement* parent, const std::string& name)
    : mName(name)
    , mActive(true)
    , mpRoot(root)
    , mpParent(parent)
{}

UIElement::~UIElement()
{}

UIElement* UIElement::AddChild(std::unique_ptr<UIElement>&& ptr)
{
    UIElement* element = ptr.get();
    mChildren.push_back(std::move(ptr));
    return element;
}

//
// Destroy a child of this element and release its memory.
//    Also recursively destroys children of this child.
//
void UIElement::DestroyChild(UIElement* childToDestroy)
{
    auto filteringLambda = [childToDestroy](const auto& child) { return child.get() == childToDestroy; };

    mChildren.erase(std::remove_if(mChildren.begin(), mChildren.end(), filteringLambda), mChildren.end());

    // Destructor of childToDestroy AND ALL ITS CHILDREN will now be called.
}

void UIElement::SetActive(bool active)
{
    if (active == mActive)
    {
        return;
    }

    mActive = active;
    for (auto& child : mChildren)
    {
        child->SetActive(active);
    }
}

void UIElement::Update(TIMEDELTA dt)
{
    for (auto& child : mChildren)
    {
        child->Update(dt);
    }
}

UIElement* UIElement::GetParent() const
{
    return mpParent;
}

}; // namespace Engine

}; // namespace CubeWorld
