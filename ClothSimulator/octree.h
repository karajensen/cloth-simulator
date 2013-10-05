////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - octree.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "callbacks.h"
#include <deque>

class CollisionMesh;

/**
* Partitions the scene into sections for effecient collision detection
*/
class Octree
{
public:

    typedef std::shared_ptr<CollisionMesh> CollisionMeshPtr;

    /**
    * Constructor
    * @param engine Callbacks from the rendering engine
    */
    explicit Octree(EnginePtr engine);

    /**
    * Forms the root partitions for the tree
    */
    void BuildInitialTree();

    /**
    * Adds a collision object to the octree
    * @param object The collision object to add
    */
    void AddObject(CollisionMeshPtr object);

    //void RemoveObject

    /**
    * Renders the octree partition diagnostics
    */
    void RenderDiagnostics();

private:

    struct Partition
    {
        Partition(float size, const D3DXVECTOR3& minPoint,
            std::shared_ptr<Partition> owner = nullptr);

        float GetSize() const;

        int level;
        std::string id;
        D3DXVECTOR3 minBounds;
        D3DXVECTOR3 maxBounds;
        std::shared_ptr<Partition> parent;
        std::deque<std::shared_ptr<Partition>> children;
        std::deque<CollisionMeshPtr> nodes;
    };

    /**
    * Prevent copying
    */
    Octree(const Octree&);
    Octree& operator=(const Octree&);

    typedef std::shared_ptr<Partition> PartitionPtr;

    bool IsPointInsidePartition(const D3DXVECTOR3& point, const PartitionPtr& partition);
    bool IsInsidePartition(const CollisionMeshPtr& object, const PartitionPtr& partition);
    bool AddToPartition(const CollisionMeshPtr& object, PartitionPtr& partition);
    void RenderPartition(const PartitionPtr& partition);
    void GenerateChildren(PartitionPtr& parent);

    EnginePtr m_engine;              ///< Callbacks for the rendering engine
    std::deque<PartitionPtr> m_octree;   ///< Octree partitioning of collision objects

};