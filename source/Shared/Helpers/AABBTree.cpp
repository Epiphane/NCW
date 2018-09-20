// By Thomas Steinke

#include "AABBTree.h"

namespace CubeWorld
{

namespace Game
{

//
// Node Mechanics.
//

const BaseAABBTree::BaseNode BaseAABBTree::BaseNode::INVALID{nullptr, ID()};

///
///
///
BaseAABBTree::BaseNode BaseAABBTree::BaseNode::parent()
{
   if (!IsValid()) { return INVALID; }
   return mTree->GetParent(id);
}

///
///
///
BaseAABBTree::BaseNode BaseAABBTree::BaseNode::left()
{
   if (!IsValid()) { return INVALID; }
   return mTree->GetLeftChild(id);
}

///
///
///
BaseAABBTree::BaseNode BaseAABBTree::BaseNode::right()
{
   if (!IsValid()) { return INVALID; }
   return mTree->GetRightChild(id);
}

///
///
///
bool BaseAABBTree::BaseNode::IsLeaf()
{
   assert(IsValid());
   BaseNodeData nodeData = mTree->mNodes[id.index()];
   return nodeData.leftChild == BaseNodeData::INVALID && nodeData.rightChild == BaseNodeData::INVALID;
}

///
///
///
bool BaseAABBTree::BaseNode::IsValid() const
{
   return mTree != nullptr && mTree->IsValid(id);
}

///
///
///
BaseAABBTree::BaseNode::operator bool() const
{
   return IsValid();
}

///
///
///
BaseAABBTree::BaseNodeData* BaseAABBTree::BaseNode::operator->() const
{
   assert(IsValid());
   return &mTree->mNodes[id.index()];
}

///
///
///
AABB& BaseAABBTree::BaseNode::aabb() const
{
   assert(IsValid());
   return mTree->mNodes[id.index()].aabb;
}

//
// Tree mechanics.
//

///
///
///
BaseAABBTree::BaseAABBTree(uint32_t /* initialSize */)
   : mRoot(BaseNodeData::INVALID)
   , mNumNodes(0)
{
}

BaseAABBTree::~BaseAABBTree()
{
}

bool BaseAABBTree::IsValid(BaseNode::ID id)
{
   return id.index() < mNumNodes && mNodeVersion[id.index()] == id.version();
}

BaseAABBTree::BaseNode BaseAABBTree::Create(AABB bounds)
{
   uint32_t index, version;
   if (mNodeFreeList.empty())
   {
      index = mNumNodes++;

      mNodes.resize(mNumNodes);
      mNodeVersion.resize(mNumNodes);
      version = mNodeVersion[index] = 1;
   }
   else
   {
      index = mNodeFreeList.back();
      mNodeFreeList.pop_back();
      version = mNodeVersion[index];
   }
   BaseNode node(this, BaseNode::ID(index, version));

   // Initialize object.
   BaseNodeData* nodeData = &mNodes[index];
   nodeData->aabb = bounds;
   nodeData->parent = BaseNodeData::INVALID;
   nodeData->leftChild = BaseNodeData::INVALID;
   nodeData->rightChild = BaseNodeData::INVALID;

   return node;
}

BaseAABBTree::BaseNode BaseAABBTree::Insert(AABB bounds)
{
   BaseNode node = Create(bounds);

   if (mRoot == BaseNodeData::INVALID)
   {
      mRoot = node.id.index();
      return node;
   }

   BaseNode cursor = GetRoot();
   while (!cursor.IsLeaf())
   {
      AABB combined = cursor.aabb().Merge(bounds);

      // Cost to add a parent node here with the combined AABB.
      float parentNodeCost = 2.0f * combined.GetSurfaceArea();
      // Minimum cost to push further down the tree - represents the size we'd
      // have to add to the existing node's AABB.
      float pushDownCost = 2.0f * (combined.GetSurfaceArea() - cursor.aabb().GetSurfaceArea());

      BaseNode left = cursor.left();
      float leftCost = pushDownCost;
      if (left.IsLeaf())
      {
         // Cost to combine with the left leaf.
         leftCost += left.aabb().Merge(bounds).GetSurfaceArea();
      }
      else
      {
         // Estimate just by adding this AABB to the left child's
         leftCost += left.aabb().Merge(bounds).GetSurfaceArea() - left.aabb().GetSurfaceArea();
      }

      BaseNode right = cursor.right();
      float rightCost = pushDownCost;
      if (right.IsLeaf())
      {
         // Cost to combine with the left leaf.
         rightCost += right.aabb().Merge(bounds).GetSurfaceArea();
      }
      else
      {
         // Estimate just by adding this AABB to the right child's
         rightCost += right.aabb().Merge(bounds).GetSurfaceArea() - right.aabb().GetSurfaceArea();
      }

      if (parentNodeCost < leftCost && parentNodeCost < rightCost)
      {
         // Add a parent node here.
         break;
      }

      // Move down to the cheapest child.
      cursor = leftCost < rightCost ? left : right;
   }

   // At this point, we want node and cursor to be siblings. Create a new parent for them.
   BaseNode parent = Create(node.aabb().Merge(cursor.aabb()));
   parent->parent = cursor->parent;
   parent->leftChild = cursor.id.index();
   parent->rightChild = node.id.index();
   node->parent = cursor->parent = parent.id.index();

   if (parent->parent == BaseNodeData::INVALID)
   {
      mRoot = parent.id.index();
   }
   else
   {
      BaseNode grandparent = parent.parent();
      if (grandparent->leftChild == cursor.id.index())
      {
         grandparent->leftChild = parent.id.index();
      }
      else
      {
         grandparent->rightChild = parent.id.index();
      }
   }

   FixTreeUpwards(parent);

   return node;
}

void BaseAABBTree::FixTreeUpwards(BaseNode cursor)
{
   while (cursor != BaseNode::INVALID)
   {
      BaseNode left = cursor.left();
      BaseNode right = cursor.right();
      assert(left && right);

      cursor->aabb = left->aabb.Merge(right->aabb);
      cursor = cursor.parent();
   }
}

void BaseAABBTree::Remove(BaseNode& /*node*/)
{
   assert(false);
}

BaseAABBTree::BaseNode BaseAABBTree::Find(BaseNode::ID /*id*/)
{
   return BaseNode::INVALID;
}

void BaseAABBTree::Defragment()
{
   assert(false);
}

BaseAABBTree::BaseNode BaseAABBTree::GetRoot()
{
   return Get(mRoot);
}

BaseAABBTree::BaseNode BaseAABBTree::GetParent(BaseNode::ID id)
{
   assert(IsValid(id));
   return Get(mNodes[id.index()].parent);
}

BaseAABBTree::BaseNode BaseAABBTree::GetLeftChild(BaseNode::ID id)
{
   assert(IsValid(id));
   return Get(mNodes[id.index()].leftChild);
}

BaseAABBTree::BaseNode BaseAABBTree::GetRightChild(BaseNode::ID id)
{
   assert(IsValid(id));
   return Get(mNodes[id.index()].rightChild);
}

BaseAABBTree::BaseNode BaseAABBTree::Get(uint32_t index)
{
   if (index == BaseNodeData::INVALID) { return BaseNode::INVALID; }
   return BaseNode(this, BaseNode::ID(index, mNodeVersion[index]));
}

}; // namespace Game

}; // namespace CubeWorld
