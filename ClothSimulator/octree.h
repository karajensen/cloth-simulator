////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - octree.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"

class Collision;

/**
* Partitions the scene into sections for effecient collision detection
*/
class Octree
{
public:

    typedef std::shared_ptr<Collision> CollisionPtr;

    /**
    * Constructor
    */
    Octree();

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

private:

    /**
    * Prevent copying
    */
    Octree(const Octree&);
    Octree& operator=(const Octree&);

    std::vector<CollisionPtr> m_dynamic; ///< Can be moved through collision resolution
    std::vector<CollisionPtr> m_static; ///< Cannot be moved through collision resolution
};