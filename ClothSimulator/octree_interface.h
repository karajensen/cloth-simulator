////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - octree_interface.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <functional>

class CollisionMesh;

/**
* Public interface for the octree partitioning class
*/
class IOctree
{
public:

    typedef std::function<void(CollisionMesh&, CollisionMesh&)> IterateOctreeFn;

    /**
    * Adds a collision object to the octree
    * @param object The collision object to add
    */
    virtual void AddObject(CollisionMesh& object) = 0;

    /**
    * Determines if the collision object is still inside its cached
    * partition and moves it to the correct partition if necessary
    * @param object The collision object to update
    */
    virtual void UpdateObject(CollisionMesh& object) = 0;

    /**
    * Removes the collision object from the octree
    * @param object The collision object to remove
    */
    virtual void RemoveObject(CollisionMesh& object) = 0;

    /**
    * Iterates through the octree and calls a set function on
    * any nodes connected to the given node through recursion
    * @param node The node to call against any iterated nodes
    */
    virtual void IterateOctree(CollisionMesh& node) = 0;

};

