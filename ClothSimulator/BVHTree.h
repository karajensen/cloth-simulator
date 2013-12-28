////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - BVHTree.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "callbacks.h"
#include "BVHTree_interface.h"

class Partition;

/**
* Bounding Volume Hierarchy Partitioning of the scene for use in collision detection
*/
class BVHTree : public IBVHTree
{
public:

    /**
    * Constructor
    * @param engine Callbacks from the rendering engine
    */
    explicit BVHTree(std::shared_ptr<Engine> engine);

    /**
    * Destructor
    */
    ~BVHTree();

    /**
    * Creates all partitions for the tree
    */
    void BuildInitialTree();

    /**
    * Sets the function to be called when recursing through the tree
    * @param iteratorFn The function to call when recursing the tree
    */
    void SetIteratorFunction(IterateTreeFn iteratorFn);

    /**
    * Renders the tree and partition diagnostics
    * @note will only render the partitions that have nodes
    */
    void RenderDiagnostics();

    /**
    * Adds a collision object to the tree
    * @param object The collision object to add
    */
    virtual void AddObject(CollisionMesh& object) override;

    /**
    * Determines if the collision object is still inside its cached
    * partition and moves it to the correct partition if necessary
    * @param object The collision object to update
    */
    virtual void UpdateObject(CollisionMesh& object) override;

    /**
    * Removes the collision object from the tree
    * @param object The collision object to remove
    */
    virtual void RemoveObject(CollisionMesh& object) override;

    /**
    * Iterates through the tree and calls a set function on
    * any nodes connected to the given node through recursion
    * @param node The node to call against any iterated nodes
    */
    virtual void IterateTree(CollisionMesh& node) override;

private:

    /**
    * Prevent copying
    */
    BVHTree(const BVHTree&);
    BVHTree& operator=(const BVHTree&);

    /**
    * Determines if a point in global coordinates exists within the partition bounds
    * @param point The point in global coordinates
    * @param partition The partition to test within
    * @return whether the point is inside the partition bounds
    */
    bool IsPointInsidePartition(const D3DXVECTOR3& point, const Partition& partition) const;

    /**
    * Determines if a corner of an OABB exists within the partition bounds
    * @param object The collision object holding the OABB
    * @param partition The partition to test within
    * @return whether a corner of the OABB is inside the partition bounds
    */
    bool IsCornerInsidePartition(const CollisionMesh& object, const Partition& partition) const;

    /**
    * Determines if all four corners of an OABB exist within the partition bounds
    * @param object The collision object holding the OABB
    * @param partition The partition to test within
    * @return whether all four corners of the OABB are inside the partition bounds
    */
    bool IsAllInsidePartition(const CollisionMesh& object, const Partition& partition) const;

    /**
    * Renders the diagnostics for a single partition
    * @param partition The partition to render diagnostics for
    * @return the number of nodes within the partition and its children
    */
    int RenderPartition(const std::unique_ptr<Partition>& partition);

    /**
    * Generates eight child partitions for a parent partition
    * @param parent The partition to generate children for
    */
    void GenerateChildren(std::unique_ptr<Partition>& parent);

    /**
    * Iterates through the tree from the node's partition to the top-most
    * parent calling the iterator function on sibiling and parent nodes
    * @param node The key node to iterate through the tree with
    * @param partition The partition to use for finding pairing nodes
    */
    void IterateUpTree(CollisionMesh& node, Partition& partition);

    /**
    * Iterates through the tree from the node's partition to the bottom-most
    * children  calling the iterator function on children nodes
    * @param node The key node to iterate through the tree with
    * @param partition The partition to use for finding pairing nodes
    */
    void IterateDownTree(CollisionMesh& node, Partition& partition);

    /**
    * Recursive searching of the tree to determine the best partition for an object
    * @param object The collision object to find a partition for
    * @param partition The current partition to check if the object is inside
    * @return the chosen partition the object is inserted into, or null if none found
    */
    Partition* FindPartition(CollisionMesh& object, Partition& partition);

    IterateTreeFn m_iteratorFn;          ///< Function to call when iterating the tree
    std::shared_ptr<Engine> m_engine;    ///< Callbacks for the rendering engine
    std::unique_ptr<Partition> m_tree;   ///< Tree partitioning of collision objects
};

