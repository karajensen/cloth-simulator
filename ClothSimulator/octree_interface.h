////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - octree_interface.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
class CollisionMesh;

/**
* Interface for the octree partitioning class
*/
class IOctree
{
public:

    /**
    * Adds a collision object to the octree
    * @param object The collision object to add
    */
    virtual void AddObject(CollisionMesh* object) = 0;

    /**
    * Determines if the object is still inside the partition
    * and moves it to the correct partition if necessary
    * @param object The collision object to update
    */
    virtual void UpdateObject(CollisionMesh* object) = 0;

    /**
    * Removes the collision object from the octree
    * @param object The collision object to remove
    */
    virtual void RemoveObject(CollisionMesh* object) = 0;

};

