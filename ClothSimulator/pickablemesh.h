////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - pickablemesh.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"
#include "geometry.h"

/**
* Base class for any mesh that is able to be picked
*/
class PickableMesh
{
public:

    typedef std::function<void(void)> MeshPickFn;

    /**
    * Constructor.
    */
    PickableMesh();

    /**
    * Destructor
    */
    virtual ~PickableMesh();

    /**
    * Set mesh pick function
    * @param fn the function to set
    */
    void SetMeshPickFunction(MeshPickFn fn);

    /**
    * Function to call upon a mesh pick
    */
    void OnPickMesh();

private:

    MeshPickFn m_meshPickFn; ///< Function to call if mesh was picked
};
