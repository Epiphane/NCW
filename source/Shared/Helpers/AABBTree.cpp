// By Thomas Steinke

#include "AABBTree.h"

namespace CubeWorld
{

namespace Game
{

//
// Node Mechanics.
//

const AABBTree::Node AABBTree::Node::INVALID{nullptr, ID()};

///
///
///
AABBTree::Node AABBTree::Node::parent()
{
   if (!IsValid()) { return INVALID; }
   return mTree->GetParent(id);
}

///
///
///
AABBTree::Node AABBTree::Node::left()
{
   if (!IsValid()) { return INVALID; }
   return mTree->GetLeftChild(id);
}

///
///
///
AABBTree::Node AABBTree::Node::right()
{
   if (!IsValid()) { return INVALID; }
   return mTree->GetRightChild(id);
}

///
///
///
bool AABBTree::Node::IsLeaf()
{
   assert(IsValid());
   NodeData nodeData = mTree->mNodes[id.index()];
   return nodeData.leftChild == NodeData::INVALID && nodeData.rightChild == NodeData::INVALID;
}

///
///
///
bool AABBTree::Node::IsValid() const
{
   return mTree != nullptr && mTree->IsValid(id);
}

///
///
///
AABBTree::Node::operator bool() const
{
   return IsValid();
}

///
///
///
AABBTree::NodeData* AABBTree::Node::operator->() const
{
   assert(IsValid());
   return &mTree->mNodes[id.index()];
}

///
///
///
void* AABBTree::Node::data() const
{
   assert(IsValid());
   return mTree->mNodes[id.index()].data;
}

///
///
///
AABB& AABBTree::Node::aabb() const
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
AABBTree::AABBTree(uint32_t /* initialSize */)
   : mRoot(NodeData::INVALID)
   , mNumNodes(0)
{
}

AABBTree::~AABBTree()
{
}

bool AABBTree::IsValid(Node::ID id)
{
   return id.index() < mNumNodes && mNodeVersion[id.index()] == id.version();
}

AABBTree::Node AABBTree::Create(AABB bounds, void* data)
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
   Node node(this, Node::ID(index, version));

   // Initialize object.
   NodeData* nodeData = &mNodes[index];
   nodeData->aabb = bounds;
   nodeData->data = data;
   nodeData->parent = NodeData::INVALID;
   nodeData->leftChild = NodeData::INVALID;
   nodeData->rightChild = NodeData::INVALID;

   return node;
}

AABBTree::Node AABBTree::Insert(AABB bounds, void* data)
{
   Node node = Create(bounds, data);

   if (mRoot == NodeData::INVALID)
   {
      mRoot = node.id.index();
      return node;
   }

   Node cursor = GetRoot();
   while (!cursor.IsLeaf())
   {
      AABB combined = cursor.aabb().Merge(bounds);

      // Cost to add a parent node here with the combined AABB.
      float parentNodeCost = 2.0f * combined.GetSurfaceArea();
      // Minimum cost to push further down the tree - represents the size we'd
      // have to add to the existing node's AABB.
      float pushDownCost = 2.0f * (combined.GetSurfaceArea() - cursor.aabb().GetSurfaceArea());

      Node left = cursor.left();
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

      Node right = cursor.right();
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
   Node parent = Create(node.aabb().Merge(cursor.aabb()), nullptr);
   parent->parent = cursor->parent;
   parent->leftChild = cursor.id.index();
   parent->rightChild = node.id.index();
   node->parent = cursor->parent = parent.id.index();

   if (parent->parent == NodeData::INVALID)
   {
      mRoot = parent.id.index();
   }
   else
   {
      Node grandparent = parent.parent();
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

void AABBTree::FixTreeUpwards(Node cursor)
{
   while (cursor != Node::INVALID)
   {
      Node left = cursor.left();
      Node right = cursor.right();
      assert(left && right);

      cursor->aabb = left->aabb.Merge(right->aabb);
      cursor = cursor.parent();
   }
}

void AABBTree::Remove(Node& /*node*/)
{
   assert(false);
}

AABBTree::Node AABBTree::Find(Node::ID /*id*/)
{
   return Node::INVALID;
}

void AABBTree::Defragment()
{
   assert(false);
}

AABBTree::Node AABBTree::GetRoot()
{
   return Get(mRoot);
}

AABBTree::Node AABBTree::GetParent(Node::ID id)
{
   assert(IsValid(id));
   return Get(mNodes[id.index()].parent);
}

AABBTree::Node AABBTree::GetLeftChild(Node::ID id)
{
   assert(IsValid(id));
   return Get(mNodes[id.index()].leftChild);
}

AABBTree::Node AABBTree::GetRightChild(Node::ID id)
{
   assert(IsValid(id));
   return Get(mNodes[id.index()].rightChild);
}

AABBTree::Node AABBTree::Get(uint32_t index)
{
   if (index == NodeData::INVALID) { return Node::INVALID; }
   return Node(this, Node::ID(index, mNodeVersion[index]));
}

}; // namespace Game

}; // namespace CubeWorld
