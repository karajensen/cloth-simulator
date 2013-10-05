
• Octree spacial partitioning
• Wall collision logic
• GJK CollisionMesh detection
• Video/update folio


9 root -> 8 children -> 8 children -> n children
Objects can exist in parent nodes if no children or exist in > 2 children

• Updating octree location when transform changed
• Adding cloth to octree
• Removing collision mesh from octree
• Collision mesh knows whether its dynamic or kinematic 
  (only collision test dynamic/dynamic, dynamic/kinematic)
• Using octree for collision detection




=================================================================
CLOTH SIMULATOR
=================================================================
mail@KaraJensen.com
www.KaraJensen.com
https://github.com/karajensen/cloth-simulator.git
=================================================================

PLATFORM: Windows
VISUAL STUDIO VERSION: 2012
VS ENVIRONMENT VARIABLES: $(DXSDK_DIR) DirectX SDK

LALT-CLICK:     Rotate the camera
LSHIFT-CLICK:   Move back/forwards with the camera
LCTRL-CLICK:    Move sideways with the camera
BACKSPACE:      Delete currently selected object
RALT-CLICK:     Allows selection of particle. Cloth diagnostics must be toggled
WASDQE:         Control the cloth in handle mode
1-4:            Toggle different cloth edge in handle mode
0:              Toggle text diagnostics
9:              Toggle collision models
8:              Toggle general diagnostics
7:              Toggle cloth diagnostics
6:              Toggle octree diagnostics
