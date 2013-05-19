// * ************************************************************************ //
// * * GFX Project Header                                                 * * //
// * *                                                                    * * //
// * * Disclaimer: This document contains code adapted from the DirectX   * * //
// * * SDK sample (MeshFromOBJ) provided by Microsoft Corporation.        * * //
// * * It may be used freely provided this disclaimer and header remains  * * //
// * * intact. The author takes no responsibility for any loss of damage  * * //
// * * arising from the use of the computer code contained herein.        * * //
// * * Where this document is submitted as part of an assessment task     * * //
// * * this noticed must remain intact and the student must make clear    * * //
// * * indication which parts have been added by themselves.              * * //
// * *                                                                    * * //
// * ********************************************************************** * //
// * ********************************************************************** * //
// * * GFXLoader.h                                                        * * //
// * * Author: Tim Wilkin                                                 * * //
// * * Created: 02/09/10     Last Modified: 05/09/10                      * * //
// * ********************************************************************** * //


#ifndef GFXLOADER_H
#define GFXLOADER_H


#include "includes.h"
#include <map>

#define	GFX_DEFAULT_SIZE	256

//VERTEX STRUCTURE
typedef struct _vertex
{
	D3DXVECTOR3		vPosition;
	D3DXVECTOR3		vNormal;
	D3DXVECTOR2		vTexCoord;
} VERTEX, *LPVERTEX;

//CLASS
class Loader
{
private:
	DWORD	CacheVertex(UINT iPos, VERTEX& rVertex);
	bool	Equivalent(VERTEX const& v1, VERTEX const& v2, FLOAT eps = 0.0001);

	WCHAR	m_wszGeometryFilename[MAX_PATH];
	bool	m_bLoaded;

	std::map<UINT,DWORD>	m_cVertexCache;
	std::vector<VERTEX>		m_vVertexData;
	std::vector<DWORD>		m_vIndexData;

public:   
	string Folder;
	Loader();
	~Loader();
	   
	void Release();
	bool LoadGeometryFromOBJ(WCHAR const* wszFilename, WCHAR const* wszDirectory = NULL);
	bool LoadGeometryFromTerrain(WCHAR const* wszFilename, WCHAR const* wszDirectory);
	bool isLoaded() const { return m_bLoaded; }

	std::vector<VERTEX>&		GetVertexData()		{ return m_vVertexData; }
	std::vector<DWORD>&			GetIndexData()		{ return m_vIndexData; }
		
};
typedef Loader GFXLOADER, *LPGFXLOADER;


#endif