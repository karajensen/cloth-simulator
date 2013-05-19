#include "mesh.h"
#include "assimpmesh.h"
#include "collision.h"
#include "Shader.h"
#include "input.h"
#include "light.h"

Mesh::Mesh():
    m_selected(false),
    m_draw(true),
    m_mesh(nullptr),
    m_texture(nullptr),
    m_shader(nullptr)
{
}

Mesh::~Mesh()
{
    if(m_texture != nullptr)
    { 
        m_texture->Release();
    }
    if(m_mesh != nullptr)
    { 
        m_mesh->Release();
    }
}

void Mesh::DrawMesh(const D3DXVECTOR3& cameraPos, const Transform& projection, const Transform& view)
{
    if(m_mesh && m_draw)
    {
        LPD3DXEFFECT effect = Shader_Manager::UseWorldShader() ? Shader_Manager::GetWorldEffect() : m_shader->GetEffect();

        effect->SetTechnique(DxConstant::DefaultTechnique);
        effect->SetFloatArray(DxConstant::CameraPosition, &(cameraPos.x), 3);

        if(m_texture != nullptr)
        {
            effect->SetTexture(DxConstant::DiffuseTexture, m_texture);
        }

        Light_Manager::SendLightingToShader(effect);

        D3DXMATRIX wit;
        D3DXMATRIX wvp = Matrix * view.Matrix * projection.Matrix;
        float det = D3DXMatrixDeterminant(&Matrix);
        D3DXMatrixInverse(&wit, &det, &Matrix);
        D3DXMatrixTranspose(&wit, &wit);

        effect->SetMatrix(DxConstant::WorldInverseTranspose, &wit);
        effect->SetMatrix(DxConstant::WordViewProjection, &wvp);
        effect->SetMatrix(DxConstant::World, &Matrix);

        UINT nPasses = 0;
        effect->Begin(&nPasses, 0);
        for(UINT iPass = 0; iPass<nPasses; ++iPass)
        {
            effect->BeginPass(iPass);
            m_mesh->DrawSubset(0);
            effect->EndPass();
        }
        effect->End();
    }
}

void Mesh::DrawCollision(const Transform& projection, const Transform& view)
{
    if(m_collision && m_draw)
    {
        m_collision->Draw(projection, view);
    }
}

Collision* Mesh::GetCollision()
{
    return m_collision.get();
}

void Mesh::MousePickingTest(Picking& input)
{
    if(m_draw && m_mesh)
    {
        LPD3DXMESH meshToTest = m_collision ? m_collision->GetMesh() : m_mesh;

        D3DXMATRIX worldInverse;
        D3DXMatrixInverse(&worldInverse, NULL, &Matrix);

        D3DXVECTOR3 rayOrigin;
        D3DXVECTOR3 rayDirection;
        D3DXVec3TransformCoord(&rayOrigin, &input.GetRayOrigin(), &worldInverse);
        D3DXVec3TransformNormal(&rayDirection, &input.GetRayDirection(), &worldInverse);
        D3DXVec3Normalize(&rayDirection, &rayDirection);

        BOOL hasHit; 
        float distanceToCollision;
        if(FAILED(D3DXIntersect(meshToTest, &rayOrigin, &rayDirection, 
            &hasHit, NULL, NULL, NULL, &distanceToCollision, NULL, NULL)))
        {
            hasHit = false; //Call failed for any reason continue to next mesh.
        }

        if(hasHit)
        {
            //if first mesh tested or if better than a previous test then save
            if(input.GetMesh() == nullptr || distanceToCollision < input.GetDistanceToMesh())
            {
                input.SetPickedMesh(this, distanceToCollision);
            }
        }
    }
}

void Mesh::SetCollisionVisibility(bool draw)
{
    if(m_collision)
    {
        m_collision->SetDraw(draw);
    }
}

void Mesh::ToggleVisibility()
{
    m_draw = !m_draw;
}

void Mesh::ToggleSelected()
{
    m_selected = !m_selected;
}

bool Mesh::Load(LPDIRECT3DDEVICE9 d3ddev, const std::string& filename, std::shared_ptr<Shader> shader)
{
    m_shader = shader;
    SetMeshPickFunction(std::bind(&Mesh::ToggleSelected, this));

    //Create a assimp mesh
    std::string errorBuffer;
    Assimpmesh mesh;
    if(!mesh.Initialise(filename, errorBuffer))
    {
        Diagnostic::ShowMessage(errorBuffer);
        return false;
    }

    std::vector<unsigned long> indexData;
    std::vector<Vertex> vertexData;
    const std::vector<Assimpmesh::SubMesh>& subMeshes = mesh.GetMeshes();

    for(unsigned int i = 0; i < subMeshes.size(); ++i)
    {
        //Create any textures
        std::string diffuse = subMeshes[i].textures[Assimpmesh::DIFFUSE];
        if(!diffuse.empty() && !m_texture)
        {
            if(FAILED(D3DXCreateTextureFromFile(d3ddev, diffuse.c_str(), &m_texture)))
            {
                Diagnostic::ShowMessage("Cannot create texture " + diffuse);
                return false;
            }
        }

        //Fill in vertex data
        for(unsigned int j = 0; j < subMeshes[i].vertices.size(); ++j)
        {
            Vertex v;
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

    //Mesh Vertex Declaration
    D3DVERTEXELEMENT9 VertexDec[] =
    {
        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },     
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    };

    //Create the DirectX Mesh
    if(FAILED(D3DXCreateMesh(indexData.size()/3,
                             vertexData.size(),
                             D3DXMESH_MANAGED | D3DXMESH_32BIT,
                             VertexDec,
                             d3ddev,
                             &m_mesh)))
    {
        Diagnostic::ShowMessage("Mesh " + filename + " creation failed");
        return false;
    }

    //Fill in the vertex buffer
    Vertex* pVertexBuffer;
    if(FAILED( m_mesh->LockVertexBuffer(0, (void**)&pVertexBuffer)))
    {
        Diagnostic::ShowMessage(filename + " Vertex buffer lock failed");
        return false;
    }
    #pragma warning(disable: 4996)
    std::copy(vertexData.begin(), vertexData.end(), pVertexBuffer);
    m_mesh->UnlockVertexBuffer();

    //Fill in the index buffer
    DWORD* pm_indexBuffer;
    if(FAILED(m_mesh->LockIndexBuffer(0, (void**)&pm_indexBuffer)))
    {
        Diagnostic::ShowMessage(filename + " Index buffer lock failed");
        return false;
    }
    #pragma warning(disable: 4996)
    std::copy(indexData.begin(), indexData.end(), pm_indexBuffer);
    m_mesh->UnlockIndexBuffer();

    return true;
}

void Mesh::CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float width, float height, float depth)
{
    CollisionCube* cube = new CollisionCube(d3ddev, *this, width, height, depth);
    m_collision.reset(cube);

    Transform::UpdateFn fullFn = std::bind(&CollisionCube::FullUpdate, cube);
    Transform::UpdateFn positionalFn = std::bind(&CollisionCube::PositionalUpdate, cube);
    SetObserver(fullFn, positionalFn);
}

void Mesh::CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float radius, int quality)
{
    CollisionSphere* sphere = new CollisionSphere(d3ddev, *this, radius, quality);
    m_collision.reset(sphere);

    Transform::UpdateFn fullFn = std::bind(&CollisionSphere::FullUpdate, sphere);
    Transform::UpdateFn positionalFn = std::bind(&CollisionSphere::PositionalUpdate, sphere);
    SetObserver(fullFn, positionalFn);
}