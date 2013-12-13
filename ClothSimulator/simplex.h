////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - simplex.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include <deque>
#include <array>

/**
* Edge between faces of a n-dimensional simplex
*/
struct Edge
{
    /**
    * Constructor
    */
    Edge();

    std::array<int, 2> indices; ///< Index for simplex points
};

/**
* Triangle face of a n-dimensional simplex
*/
struct Face
{
    /**
    * Constructor
    */
    Face();

    int index; ///< User index of face
    D3DXVECTOR3 normal; ///< Normal of the face
    float distanceToOrigin; ///< Distance of face to origin
    std::array<int, 3> indices; ///< Index for simplex points
    std::array<Edge, 3> edges; ///< Edges surrounding the face
};

/**
* Holds points an n-dimensional simplex
* For tetrahedron+ can generate and hold face information
*/
class Simplex
{
public:

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
    * @param index The index for the simplex container
    * @return The point at the given index
    */
    const D3DXVECTOR3& GetPoint(int index) const;
   
    /**
    * @return the list of all points
    */
    const std::deque<D3DXVECTOR3>& GetPoints() const;

    /**
    * Generates the initial faces of a terminating simplex
    * @note the simplex must be tetrahedron
    */
    void GenerateFaces();

    /**
    * Connects the vertices of the current face with the given point
    * @param point The point to extend to
    */
    void ExtendFace(const D3DXVECTOR3& point);

    /**
    * @return the faces for the simplex
    */
    const std::deque<Face>& GetFaces() const { return m_faces; }

    /**
    * @param faceindex The index for the face
    * @return the center point of the face
    */
    D3DXVECTOR3 GetFaceCenter(int faceindex) const;

private:

    /**
    * @param face The face to find the distance for
    * @return the distance from the face to the origin
    */
    float GetDistanceToOrigin(const Face& face) const;

    /**
    * @param edge1 The first edge to compare
    * @param edge2 The second edge to compare
    * @return whether the two edges are equal
    */
    bool AreEdgesEqual(const Edge& edge1, const Edge& edge2) const;

    /**
    * Determines if the given edge exists among the comparison faces
    * @param edge The edge to check for
    * @param comparison A list of faces indices to search for the edge
    * @return whether the given edge is shared amongst the given faces
    */
    bool IsSharedEdge(const Edge& edge, const std::vector<int>& faces) const;

    /**
    * Fills the given container with any edges from the face that are border edges
    * @param face The face to find the border edges for
    * @param faces All possible connected faces to the face
    * @param edges A container to fill with border edges
    */
    void GetBorderEdges(const Face& face, const std::vector<int>& faces, 
        std::vector<const Edge*>& edges);

    std::deque<Face> m_faces; ///< faces for tetrahedron+ simplex points
    std::deque<D3DXVECTOR3> m_simplex; ///< Internal simplex container
};