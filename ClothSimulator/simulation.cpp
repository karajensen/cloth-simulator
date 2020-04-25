////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - simulation.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "simulation.h"
#include "cloth.h"
#include "camera.h"
#include "light.h"
#include "input.h"
#include "shader.h"
#include "collisionmesh.h"
#include "timer.h"
#include "text.h"
#include "scene.h"
#include "octree.h"
#include "collisionsolver.h"

#include <algorithm>
#include <sstream>

namespace
{
    const float CAMERA_MOVE_SPEED = 40.0f;  ///< Speed the camera will translate
    const float CAMERA_ROT_SPEED = 2.0f;    ///< Speed the camera will rotate
    const float HANDLE_SPEED = 20.0f;       ///< Speed the cloth will move in handle mode

    const D3DCOLOR BACK_BUFFER_COLOR(D3DCOLOR_XRGB(190, 190, 195)); 
    const D3DCOLOR RENDER_COLOR(D3DCOLOR_XRGB(0, 0, 255));          
    const D3DCOLOR UPDATE_COLOR(D3DCOLOR_XRGB(0, 255, 0));          
}

Simulation::Simulation()
    : m_drawCollisions(false)
    , m_d3ddev(nullptr)
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
    D3DPERF_BeginEvent(RENDER_COLOR, L"Simulation::Render");

    m_d3ddev->BeginScene();
    m_d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, BACK_BUFFER_COLOR, 1.0f, 0);
    m_d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

    D3DXVECTOR3 cameraPosition(m_camera->World().Position());
    m_scene->Draw(cameraPosition, m_camera->Projection(), m_camera->View());
    m_cloth->Draw(cameraPosition, m_camera->Projection(), m_camera->View());
    m_cloth->DrawCollisions(m_camera->Projection(), m_camera->View());
    m_scene->DrawCollisions(m_camera->Projection(), m_camera->View());
    m_scene->DrawTools(cameraPosition, m_camera->Projection(), m_camera->View());
    m_octree->RenderDiagnostics();

    m_diagnostics->DrawAllObjects(m_camera->Projection(), m_camera->View());
    m_diagnostics->DrawAllText();

    m_d3ddev->EndScene();
    m_d3ddev->Present(nullptr, nullptr, nullptr, nullptr);

    D3DPERF_EndEvent();
}

void Simulation::Update()
{
    D3DPERF_BeginEvent(UPDATE_COLOR, L"Simulation::Update");

    m_timer->UpdateTimer();
    m_input->UpdateInput();
    m_camera->UpdateCamera();
    
    if (m_input->IsMouseClicked())
    {
        m_input->UpdatePicking(m_camera->Projection(), m_camera->World());
        m_scene->ManipulatorPickingTest(m_input->GetMousePicking());
        m_cloth->MousePickingTest(m_input->GetMousePicking());
        m_scene->ScenePickingTest(m_input->GetMousePicking());
        m_input->SolvePicking();
    }

    const float deltatime = m_timer->GetDeltaTime();
    const bool pressed = m_input->IsClickPreventionActive() 
        ? false : m_input->IsMousePressed();

    m_cloth->PreCollisionUpdate(deltatime);
    m_scene->PreCollisionUpdate(pressed, m_input->GetMouseDirection(),
        m_camera->World(), m_camera->InverseProjection(), deltatime);

    m_scene->SolveCollisions();
    m_cloth->PostCollisionUpdate();
    m_scene->PostCollisionUpdate();

    D3DPERF_EndEvent();
}

void Simulation::LoadGuiCallbacks(GuiCallbacks* callbacks)
{
    using namespace std::placeholders;
    m_scene->LoadGuiCallbacks(callbacks);

    callbacks->setGravity = std::bind(&Cloth::SetSimulation, m_cloth.get(), _1);
    callbacks->resetCloth = std::bind(&Cloth::Reset, m_cloth.get());
    callbacks->unpinCloth = std::bind(&Cloth::UnpinCloth, m_cloth.get());
    callbacks->resetCamera = std::bind(&Camera::Reset, m_camera.get());
    callbacks->setVertsVisible = std::bind(&Cloth::SetVertexVisibility, m_cloth.get(), _1);
    callbacks->setHandleMode = std::bind(&Cloth::SetHandleMode, m_cloth.get(), _1);
    callbacks->setWireframeMode = std::bind(&Diagnostic::SetWireframe, m_diagnostics.get(), _1);

    callbacks->createBox = std::bind(&Scene::AddObject, m_scene.get(), Scene::BOX);
    callbacks->createSphere = std::bind(&Scene::AddObject, m_scene.get(), Scene::SPHERE);
    callbacks->createCylinder = std::bind(&Scene::AddObject, m_scene.get(), Scene::CYLINDER);
    callbacks->clearScene = std::bind(&Scene::RemoveScene, m_scene.get());

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
    m_diagnostics.reset(new Diagnostic());
    m_shader.reset(new ShaderManager());
    m_light.reset(new LightManager());

    // Create the engine callbacks
    EnginePtr engine(new Engine());
    engine->device = [this](){ return m_d3ddev; };
    engine->diagnostic = [this](){ return m_diagnostics.get(); };
    engine->octree = [this](){ return m_octree.get(); };
    
    engine->getShader = std::bind(&ShaderManager::GetShader, 
        m_shader.get(), std::placeholders::_1);

    engine->sendLightsToShader = std::bind(&LightManager::SendLightsToShader,
        m_light.get(), std::placeholders::_1);

    // Initialise the camera
    const D3DXVECTOR3 position(0.0f, 0.0f, -30.0f);
    const D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    m_camera.reset(new Camera(position, target));
    m_camera->CreateProjectionMatrix();

    // Initialise the shaders/lights
    if(!m_shader->Inititalise(d3ddev) || !m_light->Inititalise())
    {
        return false;
    }

    // Initialise diagnostics
    m_diagnostics->Initialise(d3ddev,
        m_shader->GetShader(ShaderManager::BOUNDS_SHADER));

    // Initialise the octree partitioning
    Octree* octree = new Octree(engine);
    octree->BuildInitialTree();
    m_octree.reset(octree);

    // Initialise the simulation
    m_cloth.reset(new Cloth(engine));
    m_solver.reset(new CollisionSolver(engine, m_cloth));
    m_scene.reset(new Scene(engine, m_solver));

    // Hook up the solver to the octree
    octree->SetIteratorFunction(std::bind(&CollisionSolver::SolveObjectCollision, 
        m_solver.get(), std::placeholders::_1, std::placeholders::_2));

    // Initialise the input
    m_timer.reset(new Timer(engine));
    LoadInput(hInstance, hWnd, engine);
    m_timer->StartTimer();

    return true;
}

void Simulation::LoadInput(HINSTANCE hInstance, HWND hWnd, EnginePtr engine)
{
    m_input.reset(new Input(hInstance, hWnd, engine));

    // Camera forward movement
    auto cameraForward = [this]()
    {
        m_camera->ForwardMovement(m_input->GetMouseDirection(), 
            m_timer->GetDeltaTime()*CAMERA_MOVE_SPEED, 
            m_input->IsMousePressed());
    };
    m_input->SetKeyCallback(DIK_LSHIFT, true, cameraForward);
    m_input->AddClickPreventionKey(DIK_LSHIFT);
    
    // Camera side movement
    auto cameraSideways = [this]()
    {
        m_camera->SideMovement(m_input->GetMouseDirection(), 
            m_timer->GetDeltaTime()*CAMERA_MOVE_SPEED, 
            m_input->IsMousePressed());
    };
    m_input->SetKeyCallback(DIK_LCONTROL, true, cameraSideways);
    m_input->AddClickPreventionKey(DIK_LCONTROL);
    
    // Camera rotation
    auto cameraRotation = [this]()
    {
        m_camera->Rotation(m_input->GetMouseDirection(), 
            m_timer->GetDeltaTime()*CAMERA_ROT_SPEED, 
            m_input->IsMousePressed());
    };
    m_input->SetKeyCallback(DIK_LALT, true, cameraRotation);
    m_input->AddClickPreventionKey(DIK_LALT);
    
    // Controlling the cloth
    m_input->SetKeyCallback(DIK_A, true, [this](){ m_cloth->MovePinnedRow(
        -m_timer->GetDeltaTime()*HANDLE_SPEED, 0.0f, 0.0f); });
    
    m_input->SetKeyCallback(DIK_D, true, [this](){ m_cloth->MovePinnedRow(
        m_timer->GetDeltaTime()*HANDLE_SPEED, 0.0f, 0.0f); });
    
    m_input->SetKeyCallback(DIK_S, true, [this](){ m_cloth->MovePinnedRow(
        0.0f, -m_timer->GetDeltaTime()*HANDLE_SPEED, 0.0f); });
    
    m_input->SetKeyCallback(DIK_W, true, [this](){ m_cloth->MovePinnedRow(
        0.0f, m_timer->GetDeltaTime()*HANDLE_SPEED, 0.0f); });
    
    m_input->SetKeyCallback(DIK_Q, true, [this](){ m_cloth->MovePinnedRow(
        0.0f, 0.0f, -m_timer->GetDeltaTime()*HANDLE_SPEED); });
    
    m_input->SetKeyCallback(DIK_E, true, [this](){ m_cloth->MovePinnedRow(
        0.0f, 0.0f, m_timer->GetDeltaTime()*HANDLE_SPEED); });
    
    // Changing the cloth row selected
    m_input->SetKeyCallback(DIK_1, false, 
        [this](){ m_cloth->ChangeRow(1); });
    
    m_input->SetKeyCallback(DIK_2, false, 
        [this](){ m_cloth->ChangeRow(2); });
    
    m_input->SetKeyCallback(DIK_3, false, 
        [this](){ m_cloth->ChangeRow(3); });
    
    m_input->SetKeyCallback(DIK_4, false, 
        [this](){ m_cloth->ChangeRow(4); });
    
    // Scene shortcut keys
    m_input->SetKeyCallback(DIK_BACKSPACE, false,
        std::bind(&Scene::RemoveObject, m_scene.get()));

    // Cloth smoothing
    m_input->SetKeyCallback(DIK_EQUALS, true, 
        std::bind(&Cloth::ChangeSmoothing, m_cloth.get(), true));

    m_input->SetKeyCallback(DIK_MINUS, true, 
        std::bind(&Cloth::ChangeSmoothing, m_cloth.get(), false));

    // Setting deltatime explicitly
    m_input->SetKeyCallback(DIK_P, false, 
        std::bind(&Timer::ToggleForceDeltatime, m_timer.get()));

    m_input->SetKeyCallback(DIK_RBRACKET, true, 
        std::bind(&Timer::ChangeDeltatime, m_timer.get(), true));

    m_input->SetKeyCallback(DIK_LBRACKET, true, 
        std::bind(&Timer::ChangeDeltatime, m_timer.get(), false));
    
    // Toggling Diagnostic drawing
    m_input->SetKeyCallback(DIK_T, false, 
        std::bind(&Diagnostic::ToggleDiagnostics, 
        m_diagnostics.get(), Diagnostic::TEXT));
    
    m_input->SetKeyCallback(DIK_8, false, 
        std::bind(&Diagnostic::ToggleDiagnostics, 
        m_diagnostics.get(), Diagnostic::MESH));
    
    m_input->SetKeyCallback(DIK_7, false, 
        std::bind(&Diagnostic::ToggleDiagnostics,
        m_diagnostics.get(), Diagnostic::CLOTH));
  
    m_input->SetKeyCallback(DIK_6, false, 
        std::bind(&Diagnostic::ToggleDiagnostics,
        m_diagnostics.get(), Diagnostic::OCTREE));    

    m_input->SetKeyCallback(DIK_5, false, 
        std::bind(&Diagnostic::ToggleDiagnostics,
        m_diagnostics.get(), Diagnostic::COLLISION));    
    
    // Toggle mesh collision model diagnostics
    m_input->SetKeyCallback(DIK_0, false, [this]()
    {
        m_drawCollisions = !m_drawCollisions;
        m_cloth->SetCollisionVisibility(m_drawCollisions);
        m_scene->SetCollisionVisibility(m_drawCollisions);
    });

    // Toggle wall collision model diagnostics
    m_input->SetKeyCallback(DIK_9, false, 
        std::bind(&Scene::ToggleWallVisibility, m_scene.get()));   
}