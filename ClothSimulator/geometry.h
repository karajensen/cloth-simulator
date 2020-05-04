////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - meshparts.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "directx.h"

#include <array>
#include <vector>

class Diagnostic;

/**
* Polygon triangle for a collision mesh
*/
struct MeshFace
{
    /**
    * Constructor
    * @param p0/p1/p2 The three vertices of the polygon triangle
    */
    MeshFace(const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2);

    D3DXVECTOR3 u;        ///< Vector from origin to p1 on the triangle
    D3DXVECTOR3 v;        ///< Vector from origin to p2 on the triangle
    D3DXVECTOR3 normal;   ///< Normal of the polygon face
    D3DXVECTOR3 origin;   ///< Origin of the polygon face
    D3DXVECTOR3 P1, P2;   ///< Points on the mesh face
    D3DXVECTOR3 center;   ///< Center of the mesh face
    float uu;             ///< U dot U cached for performance 
    float vv;             ///< V dot V cached for performance
    float uv;             ///< U dot V cached for performance       
};

/**
* Mesh vertex structure
*/
struct MeshVertex
{
    /**
    * Constructor
    */
    MeshVertex();

    D3DXVECTOR3 position;   ///< Vertex position
    D3DXVECTOR3 normal;     ///< Vertex normal
    D3DXVECTOR2 uvs;        ///< Vertex UV information
};

/**
* D3DX Mesh Primitive Vertex structure
*/
struct D3DXVertex
{
    /**
    * Constructor
    */
    D3DXVertex();

    D3DXVECTOR3 position;   ///< Vertex position
    D3DXVECTOR3 normal;     ///< Vertex normal
};

/**
* Shared instantable Geometry
*/
class Geometry
{
public:

    /**
    * Available shapes for geometry
    */
    enum Shape
    {
        NONE,
        BOX,
        SPHERE,
        CYLINDER,
        MAX_SHAPES
    };

    /**
    * Constructor to create a geometry proxy
    * @param device The directx device
    * @param shader The shader of the mesh
    * @param shape The shape to create
    * @param divisions The divisions of the shape
    */
    Geometry(LPDIRECT3DDEVICE9 device, 
             LPD3DXEFFECT shader,
             Shape shape, 
             int divisions = 0);

    /**
    * Constructor to create a geometry mesh
    * @param device The directx device
    * @param filename The filename of the mesh
    * @param shader The shader of the mesh
    */
    Geometry(LPDIRECT3DDEVICE9 device, 
             const std::string& filename, 
             LPD3DXEFFECT shader);

    /**
    * Destructor
    */
    ~Geometry();

    /**
    * @return the shape of the geometry
    */
    Shape GetShape() const;

    /**
    * @return the mesh of the geometry
    */
    LPD3DXMESH GetMesh() const;

    /**
    * @return the texture of the geometry
    */
    LPDIRECT3DTEXTURE9 GetTexture() const;

    /**
    * @return the shader of the geometry
    */
    LPD3DXEFFECT GetShader() const;

    /**
    * @return the shader of the geometry
    */
    const std::vector<D3DXVECTOR3>& GetVertices() const;

    /**
    * @return the shader of the geometry
    */
    const std::vector<MeshFace>& GetFaces() const;

    /**
    * Loads a texture for the mesh
    * @param d3ddev the directX device
    * @param filename the filename for the texture
    * @param dimensions the size of the texture
    * @param miplevels the number of mipmap levels to generate
    */
    void LoadTexture(LPDIRECT3DDEVICE9 d3ddev, 
                     const std::string& filename, 
                     int dimensions, 
                     int miplevels);

    /**
    * Updates the diagnostics for the geometry
    * @param renderer The diagnostic renderer
    * @param world The world matrix of the mesh using the geometry
    */
    void UpdateDiagnostics(Diagnostic& renderer, const D3DXMATRIX& world);

private:

    /**
    * Fills in information for a new mesh
    * @param mesh directx mesh
    * @param saveVertices Whether to cache the vertices or not
    */
    template<typename Vertex, typename Index> 
    void CreateMeshData(bool saveVertices);

private:

    Shape m_shape;                       ///< Type of shape of the collision geometry
    LPD3DXMESH m_mesh;                   ///< Directx geometry mesh
    LPDIRECT3DTEXTURE9 m_texture;        ///< The texture attached to the mesh
    LPD3DXEFFECT m_shader;               ///< The shader attached to the mesh
    std::vector<D3DXVECTOR3> m_vertices; ///< vertices of the mesh
    std::vector<MeshFace> m_faces;       ///< Cached local faces of the mesh
};