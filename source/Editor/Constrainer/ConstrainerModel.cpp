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
   
size_t ConstrainerModel::TotalNumberOfElementsInSubtree(UIElement& subtreeRoot)
{
   size_t result = 1;
   
//   for (auto it = subtreeRoot.BeginChildren(); it != subtreeRoot.EndChildren(); it++) {
//      result += TotalNumberOfElementsInSubtree(*it);
//   }
   
   return result;
}
   
size_t ConstrainerModel::GetTotalNumElements()
{
   return TotalNumberOfElementsInSubtree(*mpBaseElement);
}

}; // namespace Constrainer

}; // namespace Editor

}; // namespace CubeWorld
