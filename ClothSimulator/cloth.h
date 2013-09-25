////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - cloth.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "mesh.h"

class Collision;
class CollisionSolver;
class Particle;
class Spring;

/**
* Dynamic mesh with soft body physics
*/
class Cloth : public PickableMesh, public Transform
{
public:

    typedef std::shared_ptr<Particle> ParticlePtr;
    typedef std::shared_ptr<Spring> SpringPtr;

    /**
    * Constructor; loads the cloth mesh
    * @param d3ddev the directX device
    * @param callbacks The callbacks for rendering a mesh
    */
    Cloth(LPDIRECT3DDEVICE9 d3ddev, const RenderCallbacks& callbacks);

    /**
    * Draw the cloth visual and collision models
    * @param projection The projection matrix
    * @param view The view matrix
    */
    void DrawCollision(const Matrix& projection, const Matrix& view);

    /**
    * Draw the visual model of the mesh
    * @param cameraPos the position of the camera in world coordinates
    * @param projection the projection matrix
    * @param view the view matrix
    */
    void DrawMesh(const D3DXVECTOR3& cameraPos, 
        const Matrix& projection, const Matrix& view);

    /**
    * Test if cloth m_vertices have been clicked
    * @param input The mouse picking input
    * @return whether this mesh was picked
    */
    bool MousePickingTest(Picking& input);

    /**
    * Updates the cloth state
    * @param deltatime the time between frames
    */
    void UpdateState(double deltatime);

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
    * Allows Diagnostic select which involes selecting a 
    * particle and displaying it's information 
    * @param set Whether diagnostic select is allowed
    */
    void SetDiagnosticSelect(bool set) { m_diagnosticSelect = set; }

    /**
    * Copies vertex data over to directX vertex buffer
    * @return whether the call succeeded or not
    */
    bool UpdateVertexBuffer();

private:

    /**
    * Recreates the cloth
    * @param rows The number of rows for the cloth
    * @param spacing The spacing between vertices
    */
    void CreateCloth(int rows, float spacing);

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
    * @param row/col The row and column of the required particle
    * @return the particle in grid at row/col
    */
    ParticlePtr& GetParticle(int row, int col);

    /**
    * Creates a normal from the given three particles
    * @param p1/p2/p3 The three vertices to generate the normal
    * @return the generated normal
    */
    D3DXVECTOR3 CalculateNormal(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3);
    
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
    * Applies smoothing to the final mesh
    */
    void SmoothCloth();

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
    D3DXVECTOR3 m_gravity;      ///< Simulated Gravity of the cloth
    bool m_diagnosticSelect;    ///< Whether to allow diagnostic selection for the cloth
    int m_diagnosticParticle;   ///< Index for the particle selected for diagnostics

    LPDIRECT3DDEVICE9 m_d3ddev;               ///< DirextX device
    std::vector<D3DXVECTOR3> m_colors;        ///< Viable colors for the particles
    std::vector<SpringPtr> m_springs;         ///< Springs connecting particles together
    std::vector<ParticlePtr> m_particles;     ///< Particles across the cloth grid
    std::vector<Vertex> m_vertexData;         ///< DirectX Vertex data
    std::vector<DWORD> m_indexData;           ///< DirectX Index data
    void* m_vertexBuffer;                     ///< Raw Pointer to DirectX Vertex Buffer
    void* m_indexBuffer;                      ///< Raw Pointer to DirectX Index Buffer
    RenderCallbacks m_callbacks;              ///< Callbacks for rendering a mesh
    std::shared_ptr<CollisionSolver> m_collision; ///< Collision solver for the cloth
    std::shared_ptr<Collision> m_template;    ///< Template collision for all particles
    std::shared_ptr<MeshData> m_data;         ///< Data for rendering/instancing the mesh
};