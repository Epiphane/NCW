// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "AABB.h"

namespace CubeWorld
{

//
// AABBTree represents a grouping of AABBs in 3-dimensional space.
//
// 2 potential implementations: 
// 1. Each node keeps track of the index of its parent and children.
//    Advantage: quicker insert/remove, avoid invalidating node references
//    Disadvantage: data locality
// 2. Tree layout decided by index
//    Advantage: Easy to debug, data locality, less memory
//    Disadvantage: Node invalidation, memory copying during tree shuffles
//
// Going with #1 for now - in an attempt to help with the disadvantage,
// the Defragment() function invalidates all node references and rebalances
// the tree in a data-optimized way.
//
class BaseAABBTree
{
public:
   struct BaseNodeData {
      AABB aabb;

      //
      // References to other nodes, by index.
      //
      static constexpr uint32_t INVALID = ~uint32_t();
      uint32_t parent = INVALID;
      uint32_t leftChild = INVALID;
      uint32_t rightChild = INVALID;
   };

   class BaseNode {
   public:
      struct ID {
      public:
         ID() : _id(0) {}
         explicit ID(uint64_t id) : _id(id) {}
         ID(uint32_t index, uint32_t version) : _id(uint64_t(index) | uint64_t(version) << 32UL) {}

         uint64_t id() const { return _id; }

         bool operator == (const ID &other) const { return _id == other._id; }
         bool operator != (const ID &other) const { return _id != other._id; }
         bool operator < (const ID &other) const { return _id < other._id; }

         uint32_t index() const { return _id & 0xffffffffUL; }
         uint32_t version() const { return _id >> 32; }
      private:
         uint64_t _id;
      };

   protected:
      friend class BaseAABBTree;
      template <typename DataType>
      friend class AABBTree;

      BaseNode(BaseAABBTree* tree) : mTree(tree), id(0) {};
      BaseNode(BaseAABBTree* tree, ID id) : mTree(tree), id(id) {};

   public:
      static const BaseNode INVALID;

   public:
      // Traversal.
      BaseNode parent();
      BaseNode left();
      BaseNode right();

      // Tree info.
      bool IsLeaf();

      // Data access.
      operator bool() const;
      bool IsValid() const;
      BaseNodeData* operator->() const;
      AABB& aabb() const;

   protected:
      BaseAABBTree* mTree;
      ID id;
   };

public:
   BaseAABBTree(uint32_t initialSize = 128);
   ~BaseAABBTree();

   //
   // Returns whether a node ID is still valid.
   //
   bool IsValid(BaseNode::ID id);

   //
   // Insert a node into the tree.
   // Return value is an iterator referencing the node.
   //
   BaseNode Insert(AABB bounds);

   //
   // Remove a node from the tree.
   // This may invalidate all other nodes.
   //
   void Remove(BaseNode& node);

   //
   // In the case of an invalidated node, use
   // this to find it again.
   //
   BaseNode Find(BaseNode::ID id);

   //
   // Reorganize the data in a cache-friendly way.
   // This invalidates all Node pointers. Use Find()
   // to relocate them.
   //
   void Defragment();

   //
   // Get a reference to the root node.
   //
   BaseNode GetRoot();

   // Traversal functions.
   BaseNode GetParent(BaseNode::ID id);
   BaseNode GetLeftChild(BaseNode::ID id);
   BaseNode GetRightChild(BaseNode::ID id);

private:
   //
   // Instantiate a node
   //
   BaseNode Create(AABB bounds);

   //
   // Get a node at the specified index.
   // Private because we want other classes to use an ID,
   // not a versionless index.
   //
   BaseNode Get(uint32_t index);

   //
   // Move upward, fixing all the parent AABBs to accommodate their children.
   //
   void FixTreeUpwards(BaseNode start);

private:
   std::vector<BaseNodeData> mNodes;

   // Index of the root node.
   uint32_t mRoot = BaseNodeData::INVALID;

   // mNumNodes is the source of truth for number of nodes registered.
   uint32_t mNumNodes = 0;
   // Version of each node
   std::vector<uint32_t> mNodeVersion;
   // List of free node slots
   std::vector<uint32_t> mNodeFreeList;
};

template <typename DataType>
class AABBTree : public BaseAABBTree
{
public:
   class Node : public BaseNode
   {
   public:
      Node(BaseNode base) : BaseNode(base.mTree, base.id) {};

      const DataType& data() const
      {
         return static_cast<AABBTree*>(mTree)->mData[id.index()];
      }

      // Traversal.
      Node parent() { return Node(BaseNode::parent()); }
      Node left() { return Node(BaseNode::left()); }
      Node right() { return Node(BaseNode::right()); }

   private:
      friend class AABBTree;
   };

public:
   //
   // Reskin functions to return a Node object
   //
   Node GetRoot() { return Node(BaseAABBTree::GetRoot()); }
   Node Find(BaseNode::ID id) { return Node(BaseAABBTree::Find(id)); }
   Node GetParent(BaseNode::ID id) { return Node(BaseAABBTree::GetParent(id)); }
   Node GetLeftChild(BaseNode::ID id) { return Node(BaseAABBTree::GetLeftChild(id)); }
   Node GetRightChild(BaseNode::ID id) { return Node(BaseAABBTree::GetRightChild(id)); }

public:
   //
   // Insert a node with attached data
   //
   Node Insert(AABB bounds, DataType data)
   {
      Node node = Node(BaseAABBTree::Insert(bounds));
      if (mData.size() <= node.id.index())
      {
         mData.resize(node.id.index() + 1);
      }
      mData[node.id.index()] = data;

      return node;
   }

private:
   std::vector<DataType> mData;
};

}; // namespace CubeWorld
