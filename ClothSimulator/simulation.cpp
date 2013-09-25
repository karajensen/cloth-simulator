////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - simulation.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "simulation.h"
#include "cloth.h"
#include "camera.h"
#include "light.h"
#include "input.h"
#include "shader.h"
#include "collision.h"
#include "timer.h"
#include "text.h"
#include "scene.h"
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

Simulation::Simulation() :
    m_d3ddev(nullptr),
    m_drawCollisions(false)
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
    m_cloth->DrawMesh(cameraPosition, m_camera->Projection(), m_camera->View());
    m_cloth->DrawCollision(m_camera->Projection(), m_camera->View());
    m_scene->DrawCollision(m_camera->Projection(), m_camera->View());
    m_scene->DrawTools(cameraPosition, m_camera->Projection(), m_camera->View());

    Diagnostic::DrawAllObjects(m_camera->Projection(), m_camera->View());
    Diagnostic::DrawAllText();

    m_d3ddev->EndScene();
    m_d3ddev->Present(NULL, NULL, NULL, NULL);

    D3DPERF_EndEvent();
}

void Simulation::Update()
{
    D3DPERF_BeginEvent(UPDATE_COLOR, L"Simulation::Update");

    double deltaTime = m_timer->UpdateTimer();
    bool pressed = m_input->IsClickPreventionActive() 
        ? false : m_input->IsMousePressed();

    m_input->UpdateInput();
    m_camera->UpdateCamera();
    m_light->UpdateLights();

    if (m_input->IsMouseClicked())
    {
        m_input->UpdatePicking(m_camera->Projection(), m_camera->World());
        m_scene->ManipulatorPickingTest(m_input->GetMousePicking());
        m_cloth->MousePickingTest(m_input->GetMousePicking());
        m_scene->ScenePickingTest(m_input->GetMousePicking());
        m_input->SolvePicking();
    }

    m_cloth->UpdateState(deltaTime);
    m_scene->UpdateState(pressed, m_input->GetMouseDirection(),
        m_camera->World(), m_camera->InverseProjection());

    m_scene->SolveClothCollision(*m_solver);
    m_solver->SolveSelfCollision();
    m_cloth->UpdateVertexBuffer();

    D3DPERF_EndEvent();
}

void Simulation::LoadGuiCallbacks(GUI::GuiCallbacks* callbacks)
{
    using namespace std::placeholders;
    m_scene->LoadGuiCallbacks(callbacks);

    callbacks->setGravity = std::bind(&Cloth::SetSimulation, m_cloth.get(), _1);
    callbacks->resetCloth = std::bind(&Cloth::Reset, m_cloth.get());
    callbacks->unpinCloth = std::bind(&Cloth::UnpinCloth, m_cloth.get());
    callbacks->resetCamera = std::bind(&Camera::Reset, m_camera.get());
    callbacks->setVertsVisible = std::bind(&Cloth::SetVertexVisibility, m_cloth.get(), _1);
    callbacks->setHandleMode = std::bind(&Cloth::SetHandleMode, m_cloth.get(), _1);

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

    callbacks->setWireframeMode = [&](bool set)
    { 
        m_d3ddev->SetRenderState(D3DRS_FILLMODE, 
            set ? D3DFILL_WIREFRAME : D3DFILL_SOLID); 
    };
}

bool Simulation::CreateSimulation(HINSTANCE hInstance, HWND hWnd, LPDIRECT3DDEVICE9 d3ddev) 
{   
    m_d3ddev = d3ddev;
    m_camera.reset(new Camera(D3DXVECTOR3(0.0f,0.0f,-30.0f), D3DXVECTOR3(0.0f,0.0f,0.0f)));
    m_camera->CreateProjMatrix();

    m_shader.reset(new ShaderManager());
    m_light.reset(new LightManager());
    bool success = m_shader->Inititalise(d3ddev) && m_light->Inititalise();

    if(success)
    {
        auto boundsShader = m_shader->GetShader(ShaderManager::BOUNDS_SHADER);
        auto clothShader = m_shader->GetShader(ShaderManager::CLOTH_SHADER);

        RenderCallbacks callbacks;
        callbacks.getWorldEffect = std::bind(&ShaderManager::GetWorldEffect, m_shader.get());
        callbacks.useWorldShader = std::bind(&ShaderManager::UseWorldShader, m_shader.get());
        callbacks.sendLightingToEffect = std::bind(&LightManager::SendLightingToShader,
            m_light.get(), std::placeholders::_1);

        callbacks.getShader = [&](ShaderManager::SceneShader shader)
            { return m_shader->GetShader(shader); };

        Diagnostic::Initialise(d3ddev, boundsShader);
        m_scene.reset(new Scene(d3ddev, callbacks));
        m_cloth.reset(new Cloth(m_d3ddev, callbacks));
        m_solver.reset(new CollisionSolver(m_cloth));
    }

    LoadInput(hInstance, hWnd);

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

    // Scene shortcut keys
    m_input->SetKeyCallback(DIK_BACKSPACE, false,
        std::bind(&Scene::RemoveObject, m_scene.get()));

    // Toggling Diagnostic drawing
    m_input->SetKeyCallback(DIK_0, false, 
        std::bind(&Diagnostic::ToggleText));

    m_input->SetKeyCallback(DIK_8, false, 
        std::bind(&Diagnostic::ToggleDiagnostics, Diagnostic::GENERAL));

    m_input->SetKeyCallback(DIK_7, false, 
        std::bind(&Diagnostic::ToggleDiagnostics, Diagnostic::CLOTH));

    m_input->SetKeyCallback(DIK_6, false, 
        std::bind(&Diagnostic::ToggleDiagnostics, Diagnostic::OCTREE));    

    // Allow diagnostic selection of particles
    m_input->SetKeyCallback(DIK_RALT, true, 
        std::bind(&Cloth::SetDiagnosticSelect, m_cloth.get(), true),
        std::bind(&Cloth::SetDiagnosticSelect, m_cloth.get(), false));

    // Toggle mesh collision model diagnostics
    m_input->SetKeyCallback(DIK_9, false, [&]()
    {
        this->m_drawCollisions = !this->m_drawCollisions;
        m_cloth->SetCollisionVisibility(this->m_drawCollisions);
        m_scene->SetCollisionVisibility(this->m_drawCollisions);
    });
}