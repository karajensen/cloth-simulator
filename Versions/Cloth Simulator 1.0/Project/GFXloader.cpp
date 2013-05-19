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
// * * GFXLoader.cpp                                                      * * //
// * * Author: Tim Wilkin                                                 * * //
// * * Created: 02/09/10     Last Modified: 05/09/10                      * * //
// * ********************************************************************** * //


#include "global.h"
#include "GFXLoader.h"



//=============================================================
//Constructor
//=============================================================
Loader::Loader() : m_vVertexData(), m_vIndexData(), m_cVertexCache(), m_bLoaded(FALSE)
{
	::ZeroMemory(m_wszGeometryFilename,sizeof(m_wszGeometryFilename));
}

//=============================================================
//Destructor
//=============================================================
Loader::~Loader()
{
	if (m_bLoaded)
		Release();
}

//=============================================================
//Release resources
//=============================================================
void Loader::Release()
{
	if( m_bLoaded )
	{
		m_vVertexData.clear();
		m_vIndexData.clear();
		m_cVertexCache.clear();
		m_bLoaded = FALSE;
	}
}

//=============================================================
//Load geometry from OBJ file
//=============================================================
bool Loader::LoadGeometryFromOBJ(WCHAR const* wszFilename, WCHAR const* wszDirectory)
{

	if( wszFilename == NULL)
		return FALSE;

	//convert folder to string
	int length = wcslen(wszDirectory);
	for(int i = 0; i < length; i++)
		Folder += char(wszDirectory[i]);

	wcscpy_s(m_wszGeometryFilename,MAX_PATH,wszFilename);

	WCHAR tempfile[MAX_PATH];
	wcscpy_s(tempfile,MAX_PATH,wszDirectory);
	wcscat_s(tempfile,MAX_PATH,wszFilename);

	std::wifstream wfsGeometryStream;
	wfsGeometryStream.open(tempfile, std::wifstream::in);
   
   if( !wfsGeometryStream.is_open() )
   {
		MessageBox(NULL, TEXT("Unable to open geometry file"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
   }

   std::vector<D3DXVECTOR3>	vPosition;
   std::vector<D3DXVECTOR3> vNormal;
   std::vector<D3DXVECTOR2> vTexCoord;


   WCHAR	wszCommand[GFX_DEFAULT_SIZE] = {0};
   DWORD dwCurSubset = 0;

   while( !wfsGeometryStream.eof() )
   {
	   wfsGeometryStream >> wszCommand;
	   if( !wfsGeometryStream )
		   break;

	   if( 0 == wcscmp( wszCommand, L"#" ) )
	   {
		   // Comment - ignore
	   }

	   else if( 0 == wcscmp( wszCommand, L"v" ) )	// Vertex Position Coordinate
	   {
		   FLOAT x, y, z;
		   wfsGeometryStream >> x >> y >> z;
		   vPosition.push_back(D3DXVECTOR3(x,y,z));
	   }

	   else if( 0 == wcscmp( wszCommand, L"vt" ) )	// Vertex Barycentric Coordinate
	   {
		   FLOAT u, v;
		   wfsGeometryStream >> u >> v;
		   vTexCoord.push_back(D3DXVECTOR2(u,v));
	   }

	   else if( 0 == wcscmp( wszCommand, L"vn" ) )	// Vertex Normal Vector
	   {
		   FLOAT a, b, c;
		   wfsGeometryStream >> a >> b >> c;
		   vNormal.push_back(D3DXVECTOR3(a,b,c));
	   }

	   else if( 0 == wcscmp( wszCommand, L"f" ) )	// Face Definition Data
	   {
		   UINT iPos, iTex, iNorm;
		   VERTEX	vVert;

		   for( UINT f = 0; f < 3; f++ )
		   {
			   ::ZeroMemory( &vVert, sizeof(VERTEX) );

			   // OBJ file format uses 1-based array indexing
			   wfsGeometryStream >> iPos;
			   vVert.vPosition = vPosition[iPos - 1];

			   if( '/' == wfsGeometryStream.peek() )
			   {
				   wfsGeometryStream.ignore();

				   if( '/' != wfsGeometryStream.peek() )
				   {
					   wfsGeometryStream >> iTex;
					   vVert.vTexCoord = vTexCoord[iTex - 1];
				   }
				   if( '/' == wfsGeometryStream.peek() )
				   {
					   wfsGeometryStream.ignore();
					   wfsGeometryStream >> iNorm;
					   vVert.vNormal = vNormal[iNorm - 1];
				   }
			   }
			   // Hash the vertex to see if we have already seen it
			   DWORD index = CacheVertex(iPos,vVert);
			   m_vIndexData.push_back(index);
		   }
		   //m_vAttributeData.push_back(dwCurSubset);
	   }

	  /* else if( 0 == wcscmp( wszCommand, L"mtllib" ) )
	   {
		   wfsGeometryStream >> m_wszMaterialFilename;
	   }

	   else if( 0 == wcscmp( wszCommand, L"usemtl" ) )
	   {

		   // Obtain the material name
		   WCHAR wszName[MAX_PATH] = {0};
		   wfsGeometryStream >> wszName;

		   // do we already have an entry within the material data 
		   bool bFound = false;
		   for( UINT iMat = 0; iMat < m_vMaterialData.size(); iMat++ )
		   {
			   if( 0==wcscmp( m_vMaterialData[iMat]->wszName, wszName ) )
			   {
				   // set the current subset to the index of the material				
				   bFound = true;
				   dwCurSubset = iMat;
				   break;
			   }
		   }
		   // otherwise add an entry into the material data
		   if( !bFound )
		   {
			   LPMATERIAL pMaterial = new MATERIAL();
			   dwCurSubset = m_vMaterialData.size();
			   // initialise this material
			   wcscpy_s(pMaterial->wszName, MAX_PATH, wszName);
			   m_vMaterialData.push_back(pMaterial);
		   }
	   }*/

	   else
	   {
		   // Unhandled/unrecognised command
	   }

	   wfsGeometryStream.ignore(1000,'\n');

   };
   wfsGeometryStream.close();
   m_cVertexCache.clear();

   // Now obtain the data for each entry in the material vector
   /*if( m_wszMaterialFilename[0] )
	   LoadMaterialFromMAT(m_wszMaterialFilename,wszDirectory);


   //convert to string
	UINT nMats = m_vMaterialData.size();
	for( UINT i = 0; i < nMats; i++)
	{
		int texlength = wcslen(m_vMaterialData[i]->wszTexture);
		for(int j = 0; j < texlength; j++)
			m_vMaterialData[i]->Texture += char(m_vMaterialData[i]->wszTexture[j]);

		int namelength = wcslen(m_vMaterialData[i]->wszName);
		for(int k = 0; k < namelength; k++)
			m_vMaterialData[i]->Name += char(m_vMaterialData[i]->wszName[k]);
	}*/


   m_bLoaded = TRUE;
   return m_bLoaded;

}

//=============================================================
//Cache vertex
//=============================================================
DWORD Loader::CacheVertex(UINT hash, VERTEX &rVertex)
{
	bool bExists = false;
	DWORD index = 0;


	// lookup the hash value in the map
	std::map<UINT,DWORD>::iterator it = m_cVertexCache.find(hash);
	if( it != m_cVertexCache.end() )
	{
		VERTEX& rCachedVertex = m_vVertexData[it->second];
		if( Equivalent(rVertex,rCachedVertex) )
		{
			index = it->second;
			bExists = true;
		}
	}

	if( !bExists )
	{
		index = m_vVertexData.size();
		m_vVertexData.push_back(rVertex);
		m_cVertexCache.insert(std::pair<UINT,DWORD>(hash,index));
	}

	return index;

}

//=============================================================
//Test vertex equalivalency
//=============================================================
bool Loader::Equivalent(VERTEX const& v1, VERTEX const& v2, FLOAT eps)
{
	// two vertices are equivalent if their positions lie
	// within a ball of radius epsilon AND their normal
	// vectors are within a ball of radius epsilon

	// Since positions are displacement vectors from the origin
	// we need only one test... whether or not two vectors are closely
	// aligned... they are if the dot product of their normal vectors
	// is very close to 1
	D3DXVECTOR3 p1, p2, n1, n2;
	D3DXVec3Normalize(&p1,&v1.vPosition);
	D3DXVec3Normalize(&p2,&v2.vPosition);
	D3DXVec3Normalize(&n1,&v1.vNormal);
	D3DXVec3Normalize(&n2,&v2.vNormal);
	if( (D3DXVec3Dot(&p1,&p2) > (1-eps)) && (D3DXVec3Dot(&n1,&n2) > (1-eps)) )
		return TRUE;
	else
		return FALSE;	
}


//=============================================================
//Load Geometry from Terrain
//=============================================================
bool Loader::LoadGeometryFromTerrain(WCHAR const* wszFilename, WCHAR const* wszDirectory)
{
	BYTE*	input;
	FILE*	pFile;
	long lSize;
	size_t result;
	int	m_iTerrainSizeX, m_iTerrainSizeZ;
	float m_iTerrainMinX = -50.0f;
	float m_iTerrainMaxX =  50.0f;
	float m_iTerrainMinZ = -50.0f;
	float m_iTerrainMaxZ =  50.0f;

	if( wszFilename == NULL)
		return FALSE;

	//convert folder to string
	int length = wcslen(wszDirectory);
	for(int i = 0; i < length; i++)
		Folder += char(wszDirectory[i]);

	wcscpy_s(m_wszGeometryFilename,MAX_PATH,wszFilename);

	WCHAR tempfile[MAX_PATH];
	wcscpy_s(tempfile,MAX_PATH,wszDirectory);
	wcscat_s(tempfile,MAX_PATH,wszFilename);

	_wfopen_s(&pFile, tempfile, L"rb" );	// Open for READ in BINARY
	if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	input = new BYTE[lSize];
	//m_pTerrainData = new VERTEX[lSize];

	m_iTerrainSizeX = m_iTerrainSizeZ = (int)sqrt( (float)lSize );

	// copy the file into the buffer:
	result = fread (input,sizeof(BYTE),lSize,pFile);
	if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

	VERTEX Vertex;

	char msgbuf[1000];
	sprintf_s(msgbuf, "%d: %d\n", m_iTerrainSizeZ, m_iTerrainSizeX);
	OutputDebugStringA(msgbuf);
	FLOAT x,y,z;

	for( int r=0; r<m_iTerrainSizeX; r++ )
	{
		x = m_iTerrainMinX + r * ((m_iTerrainMaxX-m_iTerrainMinX)/(m_iTerrainSizeX-1));
		for( int c=0; c<m_iTerrainSizeZ; c++ )
		{
			z = m_iTerrainMinZ + c * ((m_iTerrainMaxZ-m_iTerrainMinZ)/(m_iTerrainSizeZ-1));
			int i = r*m_iTerrainSizeX+c;

			y = float(input[i]/17.5-6.5);

			sprintf_s(msgbuf, "%5d: %9.5f, %9.5f, %9.5f\n", i, x, y, z );
			OutputDebugStringA(msgbuf);

			Vertex.vPosition	= D3DXVECTOR3(x,y,z);
			Vertex.vNormal		= D3DXVECTOR3(0.0f,1.0f,0.0f);
			Vertex.vTexCoord	= D3DXVECTOR2((m_iTerrainMaxX+x)/(m_iTerrainMaxX-m_iTerrainMinX),
											  (m_iTerrainMaxZ+z)/(m_iTerrainMaxZ-m_iTerrainMinZ));

			m_vVertexData.push_back(Vertex);
		}
	}

	fclose( pFile );

	SAFE_DELETE(input);

	int vertexCnt=0;
	for(int r=0;r<m_iTerrainSizeX-1;r++)
	{
		for(int c=0;c<m_iTerrainSizeZ-1;c++)
		{

			if( r==0 || c==0 || r==(m_iTerrainSizeX-1) || c==(m_iTerrainSizeZ-1) )
				m_vVertexData[vertexCnt].vNormal = D3DXVECTOR3(0.0f,1.0f,0.0f);
			else
			{
				D3DXVECTOR3 vPrevXPos = m_vVertexData[vertexCnt-1].vPosition;
				D3DXVECTOR3 vNextXPos = m_vVertexData[vertexCnt+1].vPosition;
				D3DXVECTOR3 vPrevZPos = m_vVertexData[vertexCnt-64].vPosition;
				D3DXVECTOR3 vNextZPos = m_vVertexData[vertexCnt+64].vPosition;

				D3DXVECTOR3 vDiffHalf = (vNextXPos -  vPrevXPos) / 2.0f;
				D3DXVECTOR3 vXPerpendicual =  m_vVertexData[vertexCnt].vPosition - vDiffHalf;
				vDiffHalf = (vNextZPos -  vPrevZPos) / 2.0f;
				D3DXVECTOR3 vZPerpendicual =  m_vVertexData[vertexCnt].vPosition - vDiffHalf;

				D3DXVECTOR3 vPosnNorm = vXPerpendicual + vZPerpendicual;
				D3DXVec3Normalize(&m_vVertexData[vertexCnt].vNormal,&vPosnNorm);
			}

			m_vIndexData.push_back(vertexCnt+1+64);
			m_vIndexData.push_back(vertexCnt+64);
			m_vIndexData.push_back(vertexCnt);

			m_vIndexData.push_back(vertexCnt+1);
			m_vIndexData.push_back(vertexCnt+1+64);
			m_vIndexData.push_back(vertexCnt);

			vertexCnt++;
		}
		vertexCnt++;
	}

	m_bLoaded = TRUE;

	return m_bLoaded;
}
