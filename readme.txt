
• Clean cloth member vars
• Scene mesh manipulation

Move: if clicked on tool bar, move as drag

class ObjectManager
{
public:
	
	void SetTool(Tool tool);
	void UpdateTool();

private:
	
	Tool m_activeTool;
	std::vector<MeshPtr> m_meshes;
};


• friction to cloth
• Octree spacial partitioning
• Cloth self-collision
• Soft shadow mapping?

=================================================================
CLOTH SIMULATOR
=================================================================
mail@KaraJensen.com
www.KaraJensen.com
https://github.com/karajensen/cloth-simulator.git
=================================================================

VISUAL STUDIO VERSION REQUIRED: 2010
ENVIRONMENT VARIABLES REQUIRED: $(DXSDK_DIR) DirectX SDK

LALT-CLICK:     Rotate the camera
LSHIFT-CLICK:   Move back/forwards with the camera
LCTRL-CLICK:    Move sideways with the camera
1-4:            Toggle different cloth edge in handle mode
WASDQE:         Control the cloth in handle mode

0:              Toggle text diagnostics
9:              Toggle object diagnostics
8:              Toggle collision models
7:              Toggle wireframe mode
RALT-CLICK:     Allows selection of particle. Diagnostics must be toggled
