#include "simulation.h"
#include "cloth.h"
#include "camera.h"
#include "light.h"
#include "input.h"
#include "shader.h"
#include "collision.h"
#include "timer.h"
#include "text.h"
#include "meshmanager.h"
#include "clothsolver.h"
#include <algorithm>
#include <sstream>

namespace
{
    const float CAMERA_MOVE_SPEED = 40.0f;  
    const float CAMERA_ROT_SPEED = 2.0f;    
    const float HANDLE_SPEED = 20.0f;

    const D3DCOLOR BACK_BUFFER_COLOR(D3DCOLOR_XRGB(190, 190, 195)); ///< CLear colour of the back buffer
    const D3DCOLOR RENDER_COLOR(D3DCOLOR_XRGB(0, 0, 255));          ///< Render event colour
    const D3DCOLOR UPDATE_COLOR(D3DCOLOR_XRGB(0, 255, 0));          ///< Update event colour
}

bool Simulation::sm_drawCollisions = false;

Simulation::Simulation() :
    m_d3ddev(nullptr)
{
}

Simulation::~Simulation()
{
    #ifdef _DEBUG
    OutputDebugString("Simulation::~Simulation\n");
    #endif
}

void Simulation::Render()
{
    #ifdef _DEBUG
    D3DPERF_BeginEvent(RENDER_COLOR, L"Simulation::Render");
    #endif

    m_d3ddev->BeginScene();
    m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, BACK_BUFFER_COLOR, 1.0f, 0);
    m_d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

    D3DXVECTOR3 camPos(m_camera->World.Position());
    m_scene->Draw(camPos, m_camera->Projection, m_camera->View);
    m_cloth->DrawMesh(camPos, m_camera->Projection, m_camera->View);
    m_cloth->DrawCollision(m_camera->Projection, m_camera->View);
    m_scene->DrawCollision(m_camera->Projection, m_camera->View);

    Diagnostic::Get().DrawAllObjects(m_camera->Projection, m_camera->View);
    Diagnostic::Get().DrawAllText();

    m_d3ddev->EndScene();
    m_d3ddev->Present(NULL, NULL, NULL, NULL);
}

void Simulation::Update()
{
    #ifdef _DEBUG
    D3DPERF_BeginEvent(UPDATE_COLOR, L"Simulation::Update");
    #endif

    double deltaTime = m_timer->UpdateTimer();

    m_input->UpdateInput();
    m_camera->UpdateCamera();

    if (m_input->IsMouseClicked())
    {
        m_input->UpdatePicking(m_camera->Projection, m_camera->View); 
        m_cloth->MousePickingTest(m_input->GetMousePicking());
        m_scene->MousePickingTest(m_input->GetMousePicking());
        m_input->SolvePicking();
    }

    m_cloth->UpdateState(deltaTime);
    m_scene->SolveClothCollision(*m_solver);
    m_solver->SolveSelfCollision();
    m_cloth->UpdateVertexBuffer();
}

void Simulation::LoadGuiCallbacks(GUI::GuiCallbacks* callbacks)
{
    using namespace std::placeholders;
    m_scene->SetMeshEnableCallback(callbacks->enableMeshCreation);

    callbacks->setGravity = std::bind(&Cloth::SetSimulation, m_cloth.get(), _1);
    callbacks->resetCloth = std::bind(&Cloth::Reset, m_cloth.get());
    callbacks->unpinCloth = std::bind(&Cloth::UnpinCloth, m_cloth.get());
    callbacks->resetCamera = std::bind(&Camera::Reset, m_camera.get());
    callbacks->setVertsVisible = std::bind(&Cloth::SetVertexVisibility, m_cloth.get(), _1);
    callbacks->setHandleMode = std::bind(&Cloth::SetHandleMode, m_cloth.get(), _1);
    callbacks->setWireframeMode = [&](bool set){m_d3ddev->SetRenderState(
        D3DRS_FILLMODE, set ? D3DFILL_WIREFRAME : D3DFILL_SOLID); };

    callbacks->createBox = std::bind(&MeshManager::AddObject, m_scene.get(), MeshManager::BOX);
    callbacks->createSphere = std::bind(&MeshManager::AddObject, m_scene.get(), MeshManager::SPHERE);
    callbacks->createCylinder = std::bind(&MeshManager::AddObject, m_scene.get(), MeshManager::CYLINDER);
    callbacks->clearScene = std::bind(&MeshManager::RemoveScene, m_scene.get());

    callbacks->setTimestep = std::bind(&Cloth::SetTimeStep, m_cloth.get(), _1);
    callbacks->setVertexRows = std::bind(&Cloth::SetVertexRows, m_cloth.get(), _1);
    callbacks->setIterations = std::bind(&Cloth::SetIterations, m_cloth.get(), _1);
    callbacks->setSpacing = std::bind(&Cloth::SetSpacing, m_cloth.get(), _1);

    callbacks->getSpacing = std::bind(&Cloth::GetSpacing, m_cloth.get());
    callbacks->getIterations = std::bind(&Cloth::GetIterations, m_cloth.get());
    callbacks->getVertexRows = std::bind(&Cloth::GetVertexRows, m_cloth.get());
    callbacks->getTimestep = std::bind(&Cloth::GetTimeStep, m_cloth.get());
}

bool Simulation::CreateSimulation(HINSTANCE hInstance, HWND hWnd, LPDIRECT3DDEVICE9 d3ddev) 
{   
    m_d3ddev = d3ddev;
    LoadInput(hInstance, hWnd);

    m_camera.reset(new Camera(D3DXVECTOR3(0.0f,0.0f,-30.0f), D3DXVECTOR3(0.0f,0.0f,0.0f)));
    m_camera->CreateProjMatrix();

    bool success = Shader_Manager::Inititalise(d3ddev) 
        && Light_Manager::Inititalise();

    if(success)
    {
        auto boundsShader = Shader_Manager::GetShader(Shader_Manager::BOUNDS_SHADER);
        auto meshShader = Shader_Manager::GetShader(Shader_Manager::MAIN_SHADER);
        auto clothShader = Shader_Manager::GetShader(Shader_Manager::CLOTH_SHADER);

        Collision::Initialise(boundsShader);
        Diagnostic::Initialise(d3ddev, boundsShader);

        m_scene.reset(new MeshManager(d3ddev, meshShader));
        m_cloth.reset(new Cloth(m_d3ddev, clothShader));
        m_solver.reset(new ClothSolver(m_cloth));
    }

    // Start the internal timer
    m_timer.reset(new Timer());
    m_timer->StartTimer();

    return success;
}

void Simulation::LoadInput(HINSTANCE hInstance, HWND hWnd)
{
    m_input.reset(new Input(hInstance, hWnd));

    // Camera forward movement
    auto cameraForward = [&]()
    {
        m_camera->ForwardMovement(m_input->GetMouseDirection(), 
            m_timer->GetDeltaTime()*CAMERA_MOVE_SPEED, 
            m_input->IsMousePressed());
    };
    m_input->SetKeyCallback(DIK_LSHIFT, true, cameraForward);
    m_input->AddClickPreventionKey(DIK_LSHIFT);

    // Camera side movement
    auto cameraSideways = [&]()
    {
        m_camera->SideMovement(m_input->GetMouseDirection(), 
            m_timer->GetDeltaTime()*CAMERA_MOVE_SPEED, 
            m_input->IsMousePressed());
    };
    m_input->SetKeyCallback(DIK_LCONTROL, true, cameraSideways);
    m_input->AddClickPreventionKey(DIK_LCONTROL);

    // Camera rotation
    auto cameraRotation = [&]()
    {
        m_camera->Rotation(m_input->GetMouseDirection(), 
            m_timer->GetDeltaTime()*CAMERA_ROT_SPEED, 
            m_input->IsMousePressed());
    };
    m_input->SetKeyCallback(DIK_LALT, true, cameraRotation);
    m_input->AddClickPreventionKey(DIK_LALT);

    // Controlling the cloth
    m_input->SetKeyCallback(DIK_W, true, 
        [&](){ m_cloth->MovePinnedRow(-m_timer->GetDeltaTime()*HANDLE_SPEED,0.0f,0.0f); });

    m_input->SetKeyCallback(DIK_S, true, 
        [&](){ m_cloth->MovePinnedRow(m_timer->GetDeltaTime()*HANDLE_SPEED,0.0f,0.0f); });

    m_input->SetKeyCallback(DIK_A, true, 
        [&](){ m_cloth->MovePinnedRow(0.0f,-m_timer->GetDeltaTime()*HANDLE_SPEED,0.0f); });

    m_input->SetKeyCallback(DIK_D, true, 
        [&](){ m_cloth->MovePinnedRow(0.0f,m_timer->GetDeltaTime()*HANDLE_SPEED,0.0f); });

    m_input->SetKeyCallback(DIK_Q, true, 
        [&](){ m_cloth->MovePinnedRow(0.0f,0.0f,-m_timer->GetDeltaTime()*HANDLE_SPEED); });

    m_input->SetKeyCallback(DIK_E, true, 
        [&](){ m_cloth->MovePinnedRow(0.0f,0.0f,m_timer->GetDeltaTime()*HANDLE_SPEED); });

    // Changing the cloth row selected
    m_input->SetKeyCallback(DIK_1, false, 
        [&](){ m_cloth->ChangeRow(1); }); 

    m_input->SetKeyCallback(DIK_2, false, 
        [&](){ m_cloth->ChangeRow(2); }); 

    m_input->SetKeyCallback(DIK_3, false, 
        [&](){ m_cloth->ChangeRow(3); }); 

    m_input->SetKeyCallback(DIK_4, false, 
        [&](){ m_cloth->ChangeRow(4); }); 

    // Toggling Diagnostic drawing
    m_input->SetKeyCallback(DIK_0, false, 
        [&](){ Diagnostic::ToggleText(); });

    m_input->SetKeyCallback(DIK_9, false, 
        [&](){ Diagnostic::ToggleDiagnostics(); });

    // Allow diagnostic selection of particles
    m_input->SetKeyCallback(DIK_RALT, true, 
        [&](){ m_cloth->SetDiagnosticSelect(true); },
        [&](){ m_cloth->SetDiagnosticSelect(false); });

    // Toggle mesh collision model diagnostics
    m_input->SetKeyCallback(DIK_8, false, [&]()
    {
        sm_drawCollisions = !sm_drawCollisions;
        m_cloth->SetCollisionVisibility(sm_drawCollisions);
        m_scene->SetCollisionVisibility(sm_drawCollisions);
    });
}