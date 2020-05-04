////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - simplex.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "utils.h"

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

    std::array<int, POINTS_IN_EDGE> indices; ///< Index for simplex points
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

    bool alive = false;                      ///< Whether the triangle is dead
    int index = 0;                           ///< User index of face
    D3DXVECTOR3 normal;                      ///< Normal of the face
    float distanceToOrigin = 0.0f;           ///< Distance of face to origin
    std::array<int, POINTS_IN_FACE> indices; ///< Index for simplex points
    std::array<Edge, POINTS_IN_FACE> edges;  ///< Edges surrounding the face
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
    * @return the last generated border edges for the simplex
    */
    const std::vector<Edge>& GetBorderEdges() const { return m_edges; }

    /**
    * @return the faces for the simplex
    */
    const std::deque<Face>& GetFaces() const { return m_faces; }

    /**
    * @param faceindex The index for the face
    * @return the center point of the face
    */
    D3DXVECTOR3 GetFaceCenter(int faceindex) const;

    /**
    * Finds the closest triangle face to the origin point
    * @return The closest face within the simplex to the origin
    */
    const Face& GetClosestFaceToOrigin();

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
    * @param index The face index the edge lives on
    * @param edge The edge to check for
    * @param comparison A list of faces indices to search for the edge
    * @return whether the given edge is shared amongst the given faces
    */
    bool IsSharedEdge(int index, const Edge& edge, const std::vector<int>& faces) const;

    /**
    * Fills the given container with any edges from the face that are border edges
    * @param face The face to find the border edges for
    * @param faces All possible connected faces to the face
    */
    void FindBorderEdges(const Face& face, const std::vector<int>& faces);

    /**
    * @return an index of a dead face
    */
    int GetDeadFaceIndex() const;

private:

    std::vector<Edge> m_edges; ///< Found border edges for hull generation
    std::deque<Face> m_faces; ///< faces for tetrahedron+ simplex points
    std::deque<D3DXVECTOR3> m_simplex; ///< Internal simplex container
};