
• Create wall collisions logic
• GJK collision detection
• Octree spacial partitioning
• Video/update folio


• Clean algorithm to generate root/children
• Collision models know what partition they are in for diagnostics

On collision update, calls octree update
    Check if object is still in partition
    if not, moves to correct partition

for each dynamic object
    loop through all other objects in same partition and call given functions with them





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
