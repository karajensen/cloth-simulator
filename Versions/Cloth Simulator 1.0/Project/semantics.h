
#define MAX_MESH 2
enum { mGROUND=0, mBALL };

#define MAX_HANDLE 0
enum { hLEFT=0, hRIGHT, hUP, hDOWN };

#define MAX_SHADERS 4
enum { xTEX=0, xMAIN, xHIGHLIGHT, xCLOTH };

#define MAX_LIGHTS 1
enum { lMAIN=0 };

#define MAX_TEXT 2
enum { wTITLE=0, wSTATS };

#define MAX_SPRITE 21
enum { iGRAVITY=0, iGRAB, iBOX, iCAMRESET, iCLOTHRESET, iPLUStime, iMINUStime, iPLUSdamp, iMINUSdamp, iPLUSvert, iMINUSvert,
	   iPLUSsize, iMINUSsize, iSHOWVERTS, iPLUSit, iMINUSit, iHANDLE, iKEYS, iKEYLIST, iCOLLIDE, iMOVE};