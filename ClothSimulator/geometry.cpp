////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - meshparts.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "geometry.h"
#include "assimpmesh.h"
#include "diagnostic.h"

D3DXVertex::D3DXVertex() :
    normal(0.0f, 0.0f, 0.0f),
    position(0.0f, 0.0f, 0.0f)
{
}

MeshFace::MeshFace(const D3DXVECTOR3& p0, 
                   const D3DXVECTOR3& p1, 
                   const D3DXVECTOR3& p2) :
    origin(p0),
    P1(p1),
    P2(p2)
{
    u = p1 - p0;
    v = p2 - p0;

    center.x = (p0.x + p1.x + p2.x) / 3.0f;
    center.y = (p0.y + p1.y + p2.y) / 3.0f;
    center.z = (p0.z + p1.z + p2.z) / 3.0f;

    uu = D3DXVec3Dot(&u, &u);
    vv = D3DXVec3Dot(&v, &v);
    uv = D3DXVec3Dot(&u, &v);

    D3DXVec3Cross(&normal, &u, &v);
    D3DXVec3Normalize(&normal, &normal);
}

MeshVertex::MeshVertex() :
    position(0.0f, 0.0f, 0.0f),
    normal(0.0f, 0.0f, 0.0f),
    uvs(0.0f, 0.0f)
{
}

Geometry::Geometry(LPDIRECT3DDEVICE9 d3ddev, 
                   const std::string& filename,
                   LPD3DXEFFECT shader) :
    m_shape(NONE),
    m_mesh(nullptr),
    m_shader(shader),
    m_texture(nullptr)
{
    // Create a assimp mesh
    std::string errorBuffer;
    Assimpmesh mesh;
    if(!mesh.Initialise(filename, errorBuffer))
    {
        ShowMessageBox(errorBuffer);
    }

    std::vector<unsigned long> indexData;
    std::vector<MeshVertex> vertexData;
    const std::vector<Assimpmesh::SubMesh>& subMeshes = mesh.GetMeshes();

    for(unsigned int i = 0; i < subMeshes.size(); ++i)
    {
        // Fill in vertex data
        for(unsigned int j = 0; j < subMeshes[i].vertices.size(); ++j)
        {
            MeshVertex v;
            v.normal.x = subMeshes[i].vertices[j].nx;
            v.normal.y = subMeshes[i].vertices[j].ny;
            v.normal.z = subMeshes[i].vertices[j].nz;
            v.position.x = subMeshes[i].vertices[j].x;
            v.position.y = subMeshes[i].vertices[j].y;
            v.position.z = subMeshes[i].vertices[j].z;
            v.uvs.x = subMeshes[i].vertices[j].u;
            v.uvs.y = subMeshes[i].vertices[j].v;
            vertexData.push_back(v);
        }

        for(unsigned int j = 0; j < subMeshes[i].indices.size(); ++j)
        {
            indexData.push_back(subMeshes[i].indices[j]);
        }
    }

    // Mesh Vertex Declaration
    D3DVERTEXELEMENT9 VertexDecl[] =
    {
        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },     
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    // Create the DirectX Mesh
    if(FAILED(D3DXCreateMesh(indexData.size()/3, vertexData.size(), 
        D3DXMESH_MANAGED | D3DXMESH_32BIT, VertexDecl, d3ddev, &m_mesh)))
    {
        ShowMessageBox("Mesh " + filename + " creation failed");
    }

    // Fill in the vertex buffer
    MeshVertex* vertexBuffer;
    if(FAILED(m_mesh->LockVertexBuffer(0, (void**)&vertexBuffer)))
    {
        ShowMessageBox(filename + " Vertex buffer lock failed");
    }
    #pragma warning(disable: 4996)
    std::copy(vertexData.begin(), vertexData.end(), vertexBuffer);
    m_mesh->UnlockVertexBuffer();

    // Fill in the index buffer
    DWORD* indexBuffer;
    if(FAILED(m_mesh->LockIndexBuffer(0, (void**)&indexBuffer)))
    {
        ShowMessageBox(filename + " Index buffer lock failed");
    }
    #pragma warning(disable: 4996)
    std::copy(indexData.begin(), indexData.end(), indexBuffer);
    m_mesh->UnlockIndexBuffer();

    CreateMeshData<MeshVertex, DWORD>(false);
}

Geometry::Geometry(LPDIRECT3DDEVICE9 device, 
                   LPD3DXEFFECT shader,
                   Shape shape, 
                   int divisions) :
    m_shape(shape),
    m_mesh(nullptr),
    m_texture(nullptr),
    m_shader(shader)
{
    switch(shape)
    {
    case SPHERE:
        D3DXCreateSphere(device, 1.0f, divisions, divisions, &m_mesh, nullptr);
        break;
    case BOX:
        D3DXCreateBox(device, 1.0f, 1.0f, 1.0f, &m_mesh, nullptr);
        break;
    case CYLINDER:
        D3DXCreateCylinder(device, 1.0f, 1.0f, 1.0f, divisions, 1, &m_mesh, nullptr);
        break;
    }
    CreateMeshData<D3DXVertex, WORD>(true);
}

Geometry::~Geometry()
{ 
    if(m_mesh != nullptr)
    { 
        m_mesh->Release(); 
    } 
    if(m_texture != nullptr)
    { 
        m_texture->Release();
    }
}

void Geometry::LoadTexture(LPDIRECT3DDEVICE9 device,
                           const std::string& filename, 
                           int dimensions, int miplevels)
{
    if(FAILED(D3DXCreateTextureFromFileEx(device, filename.c_str(), dimensions, 
        dimensions, miplevels, 0, D3DFMT_FROM_FILE, D3DPOOL_DEFAULT, D3DX_DEFAULT, 
        D3DX_DEFAULT, 0, 0, 0, &m_texture)))
    {
        ShowMessageBox("Cannot create texture " + filename);
    }
}

template<typename Vertex, typename Index> void Geometry::CreateMeshData(bool saveVertices)
{
    DWORD vertexNumber = m_mesh->GetNumVertices();
    DWORD faceNumber = m_mesh->GetNumFaces();
    DWORD indexNumber = faceNumber * POINTS_IN_FACE;

    m_faces.clear();
    m_faces.reserve(faceNumber);

    m_vertices.clear();
    m_vertices.reserve(vertexNumber);

    // Copy all vertices
    void* vbuffer = nullptr;
    m_mesh->LockVertexBuffer(0, &vbuffer);
    Vertex* vertexBuffer = static_cast<Vertex*>(vbuffer);
    for(DWORD i = 0; i < vertexNumber; ++i)
    {
        m_vertices.push_back(vertexBuffer[i].position);
    }
    m_mesh->UnlockVertexBuffer();

    // Create cached polygons
    void* ibuffer = nullptr;
    m_mesh->LockIndexBuffer(0, &ibuffer);
    Index* indexBuffer = static_cast<Index*>(ibuffer);
    for(DWORD i = 0; i < indexNumber; i+=3)
    {
        const D3DXVECTOR3& v0 = m_vertices[indexBuffer[i]];
        const D3DXVECTOR3& v1 = m_vertices[indexBuffer[i+1]];
        const D3DXVECTOR3& v2 = m_vertices[indexBuffer[i+2]];
        m_faces.emplace_back(v0, v1, v2);
    }
    m_mesh->UnlockIndexBuffer();

    if(saveVertices)
    {
        // Remove any duplicates as directx creates 
        // 3 vertices for every triangle
        m_vertices.erase(std::unique(m_vertices.begin(), 
            m_vertices.end()), m_vertices.end()); 
    }
    else
    {
        m_vertices.clear();
    }
}

Geometry::Shape Geometry::GetShape() const
{ 
    return m_shape;
}

LPD3DXMESH Geometry::GetMesh() const
{ 
    return m_mesh; 
}

LPDIRECT3DTEXTURE9 Geometry::GetTexture() const 
{ 
    return m_texture; 
}

LPD3DXEFFECT Geometry::GetShader() const 
{ 
    return m_shader; 
}

const std::vector<D3DXVECTOR3>& Geometry::GetVertices() const 
{ 
    return m_vertices; 
}

const std::vector<MeshFace>& Geometry::GetFaces() const 
{ 
    return m_faces;
}

void Geometry::UpdateDiagnostics(Diagnostic& renderer, const D3DXMATRIX& world)
{
    if(renderer.AllowDiagnostics(Diagnostic::MESH))
    {
        std::string id = StringCast(this);
        const auto& faces = GetFaces();
        const float normalsize = 0.6f;
        for(unsigned int i = 0; i < faces.size(); ++i)
        {
            D3DXVECTOR3 center, normal;
            D3DXVec3TransformCoord(&center, &faces[i].center, &world);
            D3DXVec3TransformNormal(&normal, &faces[i].normal, &world);
            D3DXVec3Normalize(&normal, &normal);

            renderer.UpdateLine(Diagnostic::MESH, "FaceNormal" + 
                StringCast(i) + id, Diagnostic::CYAN, 
                center, center + (normal * normalsize));
        }
    }
}