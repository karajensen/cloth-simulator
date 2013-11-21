////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - simplex.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include <deque>

/**
* Holds at most four points for a line, triplane or tetrahedron simplex
*/
class Simplex
{
public:

    /**
    * The indices for the various points in the simplex
    * @note depending on the simplex size, these points may not exist
    * @note point A will always be the last point in the simplex
    */
    enum SimplexPoint
    {
        FIRST,
        SECOND,
        THIRD,
        LAST
    };

    /**
    * Constructor
    */
    Simplex();

    /**
    * @return whether the simplex is a line
    */
    bool IsLine() const;

    /**
    * @return whether the simplex is a plane
    */
    bool IsTriPlane() const;

    /**
    * @return whether the simplex is a tetrahedron
    */
    bool IsTetrahedron() const;

    /**
    * @param point The point to add to the simplex
    */
    void AddPoint(const D3DXVECTOR3& point);

    /**
    * @param point The point to remove from the simplex
    */
    void RemovePoint(const D3DXVECTOR3& point);

    /**
    * @param point The index for the simplex container
    * @return The point at the given index
    */
    const D3DXVECTOR3& GetPoint(SimplexPoint point) const;
   

private:

    std::deque<D3DXVECTOR3> m_simplex; ///< Internal simplex container

};