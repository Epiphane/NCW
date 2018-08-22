// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "AABB.h"

namespace CubeWorld
{

namespace Game
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
class AABBTree
{
public:
   struct NodeData {
      AABB aabb;
      void* data;

      //
      // References to other nodes, by index.
      //
      static constexpr uint32_t INVALID = ~uint32_t();
      uint32_t parent = INVALID;
      uint32_t leftChild = INVALID;
      uint32_t rightChild = INVALID;
   };

   class Node {
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

   private:
      friend class AABBTree;

      Node(AABBTree* tree) : mTree(tree), id(0) {};
      Node(AABBTree* tree, ID id) : mTree(tree), id(id) {};

   public:
      static const Node INVALID;

   public:

      // Traversal.
      Node parent();
      Node left();
      Node right();

      // Tree info.
      bool IsLeaf();

      // Data access.
      operator bool() const;
      bool IsValid() const;
      NodeData* operator->() const;
      void* data() const;
      AABB& aabb() const;

   private:
      AABBTree* mTree;
      ID id;
   };

public:
   AABBTree(uint32_t initialSize = 128);
   ~AABBTree();

   //
   // Returns whether a node ID is still valid.
   //
   bool IsValid(Node::ID id);

   //
   // Insert a node into the tree.
   // Return value is an iterator referencing the node.
   //
   Node Insert(AABB bounds, void* data);

   //
   // Remove a node from the tree.
   // This may invalidate all other nodes.
   //
   void Remove(Node& node);

   //
   // In the case of an invalidated node, use
   // this to find it again.
   //
   Node Find(Node::ID id);

   //
   // Reorganize the data in a cache-friendly way.
   // This invalidates all Node pointers. Use Find()
   // to relocate them.
   //
   void Defragment();

   //
   // Get a reference to the root node.
   //
   Node GetRoot();

   // Traversal functions.
   Node GetParent(Node::ID id);
   Node GetLeftChild(Node::ID id);
   Node GetRightChild(Node::ID id);

private:
   //
   // Instantiate a node
   //
   Node Create(AABB bounds, void* data);

   //
   // Get a node at the specified index.
   // Private because we want other classes to use an ID,
   // not a versionless index.
   //
   Node Get(uint32_t index);

   //
   // Move upward, fixing all the parent AABBs to accommodate their children.
   //
   void FixTreeUpwards(Node start);

private:
   std::vector<NodeData> mNodes;

   // Index of the root node.
   uint32_t mRoot = NodeData::INVALID;

   // mNumNodes is the source of truth for number of nodes registered.
   uint32_t mNumNodes = 0;
   // Version of each node
   std::vector<uint32_t> mNodeVersion;
   // List of free node slots
   std::vector<uint32_t> mNodeFreeList;
};

}; // namespace Game

}; // namespace CubeWorld
