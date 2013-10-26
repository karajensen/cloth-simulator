////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - octree.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "callbacks.h"
#include "octree_interface.h"

class Partition;

/**
* Partitions the scene into sections for use in collision detection
*/
class Octree : public IOctree
{
public:

    /**
    * Constructor
    * @param engine Callbacks from the rendering engine
    */
    explicit Octree(std::shared_ptr<Engine> engine);

    /**
    * Destructor
    */
    ~Octree();

    /**
    * Forms the nine root partitions for the tree
    */
    void BuildInitialTree();

    /**
    * Renders the octree diagnostics
    * @note will only render the partitions that have nodes
    */
    void RenderDiagnostics();

    /**
    * Adds a collision object to the octree
    * @param object The collision object to add
    */
    virtual void AddObject(CollisionMesh* object) override;

    /**
    * Determines if the object is still inside the partition
    * and moves it to the correct partition if necessary
    * @param object The collision object to update
    */
    virtual void UpdateObject(CollisionMesh* object) override;

    /**
    * Removes the collision object from the octree
    * @param object The collision object to remove
    */
    virtual void RemoveObject(CollisionMesh* object) override;

private:

    /**
    * Prevent copying
    */
    Octree(const Octree&);
    Octree& operator=(const Octree&);

    /**
    * Determines if a point in global coordinates exists within the partition bounds
    * @param point The point to test against
    * @param partition The partition to test against
    * @return whether the point is inside the partition bounds
    */
    bool IsPointInsidePartition(const D3DXVECTOR3& point, const Partition* partition);

    /**
    * Determines if a corner of an AABB exists within the partition bounds
    * @param object The collision object holding the AABB
    * @param partition The partition to test against
    * @return whether a corner of the AABB is inside the partition bounds
    */
    bool IsCornerInsidePartition(const CollisionMesh* object, const Partition* partition);

    /**
    * Determines if all four corners of an AABB exist within the partition bounds
    * @param object The collision object holding the AABB
    * @param partition The partition to test against
    * @return whether all four corners of the AABB are inside the partition bounds
    */
    bool IsAllInsidePartition(const CollisionMesh* object, const Partition* partition);

    /**
    * Renders the diagnostics for a partition and its children
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
    * Recursive searching of the octree to determine the best position for an object
    * @param object The collision object to add
    * @param partition The current partition to search
    * @return the chosen partition the object is inserted into, or null if none found
    */
    Partition* FindPartition(CollisionMesh* object, Partition* partition);

    std::shared_ptr<Engine> m_engine;      ///< Callbacks for the rendering engine
    std::unique_ptr<Partition> m_octree;   ///< Octree partitioning of collision objects
};

