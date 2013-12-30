////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - cloth.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "callbacks.h"
#include "pickablemesh.h"
#include "geometry.h"

class Picking;
class CollisionMesh;
class Particle;
class Spring;

/**
* Dynamic mesh with soft body physics
*/
class Cloth : public PickableMesh, public Transform
{
public:

    typedef std::unique_ptr<Particle> ParticlePtr;
    typedef std::unique_ptr<Spring> SpringPtr;

    /**
    * Constructor; loads the cloth mesh
    * @param engine Callbacks from the rendering engine
    */
    explicit Cloth(EnginePtr engine);

    /**
    * Destructor
    */
    ~Cloth();

    /**
    * Draw the cloth visual and collision models
    * @param projection The projection matrix
    * @param view The view matrix
    */
    void DrawCollisions(const Matrix& projection, const Matrix& view);

    /**
    * Draw the visual model of the mesh
    * @param cameraPos the position of the camera in world coordinates
    * @param projection the projection matrix
    * @param view the view matrix
    */
    void Draw(const D3DXVECTOR3& cameraPos, 
        const Matrix& projection, const Matrix& view);

    /**
    * Test if cloth m_vertices have been clicked
    * @param input The mouse picking input
    * @return whether this mesh was picked
    */
    bool MousePickingTest(Picking& input);

    /**
    * Updates the cloth state before collision solving
    * @param deltatime The time passed since last frame in seconds
    */
    void PreCollisionUpdate(float deltatime);

    /**
    * Resets the cloth to its initial state
    */
    void Reset();
    
    /**
    * @param simulating Set whether the cloth is simulating
    */
    void SetSimulation(bool simulating) { m_simulation = simulating; }

    /**
    * @return whether the cloth is simulating
    */
    bool IsSimulating() const { return m_simulation; }

    /**
    * Sets the size of the cloth  between vertices
    * @param size The spacing to set
    */
    void SetSpacing(double size);

    /**
    * @return the size of the cloth between vertices
    */
    double GetSpacing() const;

    /**
    * Sets the amount of vertex rows for the cloth
    * @param number The vertex count to set to
    */
    void SetVertexRows(double number);

    /**
    * @return the amount of vertex rows for the cloth
    */
    double GetVertexRows() const;

    /**
    * Sets the particle solver iteration amount
    * @param iterations The iterations to set to
    */
    void SetIterations(double iterations);

    /**
    * @return the particle solver iteration amount
    */
    double GetIterations() const;

    /**
    * Sets the cloth timestep
    * @param timestep The timestep to set to
    */
    void SetTimeStep(double timestep);

    /**
    * @return the timestep for the simulation
    */
    double GetTimeStep() const;

    /**
    * @return the container of cloth particles
    */
    std::vector<ParticlePtr>& GetParticles();
    
    /**
    * @param draw Set whether the vertices are visible or not
    */
    void SetVertexVisibility(bool draw);

    /**
    * @param draw Set whether the collision meshes are visible or not
    */
    void SetCollisionVisibility(bool draw);

    /**
    * @param set Shether handle mode is active or not
    */
    void SetHandleMode(bool set);

    /**
    * Remove any pinned vertices
    */
    void UnpinCloth();

    /**
    * Switches the currently selected row
    * @param row The row to switch to
    */
    void ChangeRow(int row);

    /**
    * Send a force to the currently selected row of the cloth
    * @param right/up/forward the direction and amount of force
    */
    void MovePinnedRow(float right, float up, float forward);

    /**
    * Selects the given particle
    * @param index The particle to select
    */
    void SelectParticle(int index);

    /**
    * Whether to increase or decrease the amount of
    * general overall smoothing for the cloth
    */
    void ChangeSmoothing(bool increase);

    /**
    * Copies smoothed vertex data over to directX vertex buffer 
    * @return whether the call succeeded or not
    */
    bool UpdateVertexBuffer();

    /**
    * Updates anything that requires collision update before
    */
    void PostCollisionUpdate();

private:

    /**
    * Recreates the cloth
    * @param rows The number of rows for the cloth
    * @param spacing The spacing between vertices
    */
    void CreateCloth(int rows, float spacing);

    /**
    * Draws and updates the diagnostics for the cloth
    */
    void UpdateDiagnostics();

    /**
    * Smooths the cloth vertices
    */
    void SmoothCloth();

    /**
    * Updates the cloth normals
    */
    void UpdateNormals();

    /**
    * Updates all generic vertices of the cloth
    */
    void UpdateVertices();

    /**
    * Updates the extra vertices if subdivision is required
    */
    void UpdateSubdividedVertices();

    /**
    * @param force Adds a force to each vertex in the cloth
    */
    void AddForce(const D3DXVECTOR3& force);

    /**
    * Selects the given particle for diagnostic purposes
    * @param index The particle to select
    */
    void SelectParticleForDiagnostics(int index);

    /**
    * Changes a particular row
    * @param row The row to change
    * @param select Whether to select/deselect the row
    */
    void ChangeRow(int row, bool select);

    /**
    * @param row/column The row and column of the required particle
    * @return the particle in grid at row/col
    */
    ParticlePtr& GetParticle(int row, int column);

    /**
    * Creates a normal from the given three particles
    * @param p1/p2/p3 The three vertices to generate the normal
    * @return the generated normal
    */
    D3DXVECTOR3 CalculateNormal(const D3DXVECTOR3& p1,
        const D3DXVECTOR3& p2, const D3DXVECTOR3& p3);
    
    /**
    * Adds a force to the given particle
    * @param particle Particle to add the force to
    * @param force The force to add
    */
    void AddForce(const ParticlePtr& particle, const D3DXVECTOR3& force);

    /**
    * @param particle The particle to set the color for
    */
    void SetParticleColor(const ParticlePtr& particle);

    /**
    * Prevent copying
    */
    Cloth(const Cloth&);
    Cloth& operator=(const Cloth&);

    int m_selectedRow;          ///< Current row selected when in manipulate mode
    float m_timestep;           ///< Cloth physics timestep
    float m_timestepSquared;    ///< Cloth timestep squared
    float m_damping;            ///< Damping to apply to movement of particles
    int m_springCount;          ///< Number of springs in cloth
    int m_springIterations;     ///< Number of solver iterations per tick
    int m_particleLength;       ///< Number of particles in a row/column
    int m_particleCount;        ///< Overall number of particles in the cloth
    int m_quadVertices;         ///< Number of vertices that center each quad
    bool m_simulation;          ///< Whether the cloth is currently simulating
    bool m_drawVisualParticles; ///< Whether particle visual models are drawn
    bool m_drawColParticles;    ///< Whether particle collision models are drawn
    float m_spacing;            ///< Current spacing between vertices
    bool m_handleMode;          ///< Whether the simulation is in handle mode
    bool m_subdivideCloth;      ///< Whether to subdivide the cloth or not
    D3DXVECTOR3 m_gravity;      ///< Simulated Gravity of the cloth
    float m_generalSmoothing;   ///< General overall smoothing of the cloth
    int m_diagnosticParticle;   ///< Particle for rendering diagnostics

    EnginePtr m_engine;                           ///< Callbacks for the rendering engine
    std::vector<D3DXVECTOR3> m_colors;            ///< Viable colors for the particles
    std::vector<SpringPtr> m_springs;             ///< Springs connecting particles together
    std::vector<ParticlePtr> m_particles;         ///< Particles across the cloth grid
    std::vector<MeshVertex> m_vertexData;         ///< DirectX Vertex data
    std::vector<DWORD> m_indexData;               ///< DirectX Index data
    std::shared_ptr<CollisionMesh> m_template;    ///< Template collision for all particles
    LPD3DXMESH m_mesh;                            ///< Directx geometry mesh
    LPDIRECT3DTEXTURE9 m_texture;                 ///< The texture attached to the mesh
    LPD3DXEFFECT m_shader;                        ///< The shader attached to the mesh
};