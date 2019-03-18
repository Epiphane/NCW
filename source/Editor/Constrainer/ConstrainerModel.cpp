//
// ConstrainerModel.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "ConstrainerModel.h"

namespace CubeWorld
{

namespace Editor
{

namespace Constrainer
{

ConstrainerModel::ConstrainerModel()
      : mpBaseElement(nullptr)
      , mFileSyncer("lol.txt")
{
}

UIElement* ConstrainerModel::GetBaseElement() const
{
   return mpBaseElement;
}

void ConstrainerModel::SetBaseElement(UIElement* baseElement)
{
   mpBaseElement = baseElement;
   if (mModelUpdatedCallback) {
      mModelUpdatedCallback();
   }
}

void ConstrainerModel::SetModelUpdatedCallback(const std::function<void()> &modelUpdatedCallback)
{
   mModelUpdatedCallback = modelUpdatedCallback;
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld
