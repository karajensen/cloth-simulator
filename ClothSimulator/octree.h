////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - octree.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "callbacks.h"
#include <list>

class CollisionMesh;

/**
* Partitions the scene into sections for effecient CollisionMesh detection
*/
class Octree
{
public:

    typedef std::shared_ptr<CollisionMesh> CollisionMeshPtr;

    /**
    * Constructor; loads the cloth mesh
    * @param engine Callbacks from the rendering engine
    */
    explicit Octree(EnginePtr engine);

    /**
    * Forms the initial tree of objects
    */
    void BuildInitialTree();

    /**
    * Adds a CollisionMesh object to the octree
    * @param object The CollisionMesh object to add
    * @param dynamic Whether the object is static or dynamic
    */
    void AddObject(CollisionMeshPtr object, bool dynamic);

    /**
    * Renders the octree partition diagnostics
    */
    void RenderDiagnostics();

private:

    struct Node
    {
        CollisionMeshPtr CollisionMesh;
        bool dynamic;
    };

    struct Partition
    {
        std::string id;
        Partition* parent;
        std::list<Partition> children;
        std::list<Node> nodes;
        D3DXVECTOR3 minBounds;
        D3DXVECTOR3 maxBounds;
        float size;
    };

    /**
    * Prevent copying
    */
    Octree(const Octree&);
    Octree& operator=(const Octree&);

    void RenderPartition(int recursionLevel, const Partition& partition);
    void GenerateChildren(Partition& parent);

    EnginePtr m_engine;              ///< Callbacks for the rendering engine
    std::list<Partition> m_octree;   ///< Octree partitioning of CollisionMesh objects

};