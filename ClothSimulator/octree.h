////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - octree.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "callbacks.h"
#include <list>

class Collision;

/**
* Partitions the scene into sections for effecient collision detection
*/
class Octree
{
public:

    typedef std::shared_ptr<Collision> CollisionPtr;

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
    * Adds a collision object to the octree
    * @param object The collision object to add
    * @param dynamic Whether the object is static or dynamic
    */
    void AddObject(CollisionPtr object, bool dynamic);

    /**
    * Updates the world transform of the octree
    * This allows it to move/scale with the cloth
    * @param transform The world transform to update to
    */
    void UpdateTransform(const Transform& transform);

    /**
    * Renders the octree partition diagnostics
    */
    void RenderDiagnostics();

private:

    /**
    * Prevent copying
    */
    Octree(const Octree&);
    Octree& operator=(const Octree&);

    struct Node
    {
        CollisionPtr collision;
        bool dynamic;
    };

    struct Partition
    {
        std::string id;
        Partition* parent;
        std::list<Partition> child;
        std::list<Node> nodes;
        D3DXVECTOR3 minBounds;
        D3DXVECTOR3 maxBounds;
    };

    EnginePtr m_engine;              ///< Callbacks for the rendering engine
    std::list<Partition> m_octree;   ///< Octree partitioning of collision objects
    Transform m_world;               ///< World transform for scaling/centering the octree

};