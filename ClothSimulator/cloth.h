/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Single cloth class for application
*****************************************************************/

#pragma once

#include "common.h"
#include "mesh.h"

class Particle;
class Shader;
class Collision;
struct Spring;

class Cloth : public Mesh
{
public:

    /**
    * Constructor; loads the cloth mesh
    * @param d3ddev the directX device
    * @param texture the path to the cloth texture
    * @param shader the shader for the mesh
    * @param the width/length of the cloth
    * @param the scale of the cloth
    */
    Cloth(LPDIRECT3DDEVICE9 d3ddev, const std::string& texture, 
        std::shared_ptr<Shader> shader, int dimensions, float scale);

    /**
    * Draw the cloth visual and collision models
    * @Param the projection matrix
    * @param the view matrix
    */
    virtual void DrawCollision(const Transform& projection, const Transform& view) override;

    /**
    * Test if cloth m_vertices have been clicked
    * @param the mouse picking input
    */
    virtual void MousePickingTest(Picking& input) override;

    /**
    * Updates the cloth state
    * @param delta time
    */
    void UpdateState(double deltatime);

    /**
    * Resets the cloth to its initial state
    */
    void Reset();

    /**
    * Toggle simulate cloth
    */
    void ToggleSimulation();

    /**
    * Adds a force to each vertex in the cloth
    * @param the force to add
    */
    void AddForce(const FLOAT3& force);

    /**
    * Solves collision between the cloth and an object
    * @param the object to test against
    */
    void SolveCollision(const Collision* object);

    /**
    * Set/Get self collisions for the cloth
    */
    void SetSelfCollide(bool selfCollide) { m_selfCollide = selfCollide; }
    bool IsSelfColliding() const { return m_selfCollide; }

    /**
    * Set/Get manipulate mode for the cloth
    */
    void SetManipulate(bool manip);
    bool IsManipulating() const { return m_manipulate; }
    
    /**
    * Set/Get simulate for the cloth
    */
    void SetSimulation(bool sim){ m_simulation = sim; }
    bool IsSimulating() const { return m_simulation; }

    /**
    * Set/Get Number of spring iterations for the cloth
    */
    void SetSpringIterations(int iterations) { m_springIterations = iterations; }
    int GetSpringIterations() const { return m_springIterations; }

    /**
    * Set/Get The damping on the cloth
    */
    void SetDamping(float damping) { m_damping = damping; }
    float GetDamping() const { return m_damping; }

    /**
    * Set/Get The cloth timestep
    */
    void SetTimeStep(float timestep);
    float GetTimeStep() const { return m_timestep; }

    /**
    * Get number of vertices for the cloth
    */
    unsigned int GetVertexCount() const { return m_vertexCount; }
    
    /**
    * Toggle whether vertex visual models are shown
    */
    void ToggleVisualParticles();

    /**
    * Toggle whether vertex visual models are shown
    */
    virtual void SetCollisionVisibility(bool draw) override;

    /**
    * Remove any pinned vertices
    */
    void UnpinCloth();

    /**
    * Switches the currently selected row
    * @param the row to switch to
    */
    void ChangeRow(int row);

    /**
    * Send a force to the currently selected row of the cloth
    * @param right,up,forward the direction and amount of force
    */
    void MovePinnedRow(float right, float up, float forward);

    /**
    * Selects the given particle
    * @param the particle to select
    */
    void SelectParticle(int index);

    /**
    * Selects the given particle for diagnostic purposes
    * @param the particle to select
    */
    void SelectParticleForDiagnostics(int index);

    /**
    * Allows Diagnostic select which involes selecting a 
    * particle and displaying it's information 
    * @param sets whether diagnostic select is allowed
    */
    void SetDiagnosticSelect(bool set) { m_diagnosticSelect = set; }

    /**
    * Copies vertex data over to directX vertex buffer
    * @return whether the call succeeded or not
    */
    bool UpdateVertexBuffer();

private:

    typedef std::shared_ptr<Particle> ParticlePtr;
    typedef std::shared_ptr<Spring> SpringPtr;

    /**
    * Changes a particular row
    * @param the row to change
    * @param whether to select/deselect the row
    */
    void ChangeRow(int row, bool select);

    /**
    * @return the particle in grid at row/col
    */
    ParticlePtr& GetParticle(int row, int col);

    /**
    * @return the vertex in grid at row/col
    */
    Vertex& GetVertex(int row, int col);

    /**
    * Updates the normals of the cloth for smooth shading
    */
    void UpdateNormals();

    /**
    * Creates a normal from the given three particles
    * @param the three vertices to generate the normal
    * @return the generated normal
    */
    D3DXVECTOR3 CalculateTriNormal(const ParticlePtr& p1, const ParticlePtr& p2, const ParticlePtr& p3);
    
    /**
    * Adds a force to the given particle
    * @param particle to add the force to
    * @param the force to add
    */
    void AddForce(const ParticlePtr& particle, const FLOAT3& force);

    /**
    * @param the particle to set the color for
    * @param whether to set color for diagnostics or not
    */
    void SetParticleColor(const ParticlePtr& particle);

    /**
    * Not implemented
    */
    Cloth(const Cloth&);              
    Cloth& operator=(const Cloth&);

    int m_selectedRow;          ///< Current row selected when in manipulate mode
    float m_timestep;           ///< Cloth physics timestep
    float m_timestepSquared;    ///< Cloth timestep squared
    float m_damping;            ///< Damping to apply to movement of particles
    int m_springCount;          ///< Number of springs in cloth
    int m_springIterations;     ///< Number of solver iterations per tick
    int m_vertexLength;         ///< Length between particles in the cloth
    int m_vertexWidth;          ///< Width between particles in the cloth
    int m_vertexCount;          ///< Overall number of particles in the cloth
    bool m_simulation;          ///< Whether the cloth is currently simulating
    bool m_manipulate;          ///< Whether manipulate mode is currently on
    bool m_selfCollide;         ///< Whether the cloth is currently self-colliding
    bool m_drawVisualParticles; ///< Whether particle visual models are drawn
    bool m_drawColParticles;    ///< Whether particle collision models are drawn
    FLOAT3 m_downwardPull;      ///< Simulated 'Gravity' of the cloth

    bool m_diagnosticSelect;    ///< Whether to allow diagnostic selection for the cloth
    int m_diagnosticParticle;   ///< Index for the particle selected for diagnostics

    std::vector<D3DXVECTOR3> m_colors;      ///< Viable colors for the particles
    std::vector<SpringPtr> m_springs;       ///< Springs connecting particles together
    std::vector<ParticlePtr> m_particles;   ///< Particles across the cloth grid
    std::vector<Vertex> m_vertexData;       ///< DirectX Vertex data
    std::vector<DWORD> m_indexData;         ///< DirectX Index data
    void* m_vertexBuffer;                   ///< Raw Pointer to DirectX Vertex Buffer
    void* m_indexBuffer;                    ///< Raw Pointer to DirectX Index Buffer
};