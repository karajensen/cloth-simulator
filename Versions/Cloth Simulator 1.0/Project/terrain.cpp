#include "global.h"
#include "terrain.h"

float TERRAIN::HeightAt( FLOAT px, FLOAT pz)
{
	// Part B
	// Question 4
	// Implement a function to obtain the height within the above vertex data set given three vertex values
	// and the X and Z coordinate values
	//
	// The triangle represented by the following vertices V0, V1, V2 (as per diagram below) defines a plane
	//
	//  V1 *------* V2
	//     |     /
	//     |    /
	//     |   /
	//     |  /
	//     | /
	//     |/
	//  V0 *
	//
	// This plane satisfies the equation  n.(p-V0)=0 (equivalent to n.p+d=0, d=-n.V0) for all points p in the plane
	// n is the unit normal vector to the plane, defined as
	//      (V1-V0)x(V2-V0)
	//  n = ---------------      (where 'x' is the vector cross product operator)
	//     |(V1-V0)x(V2-V0)|
	//
	// The resulting normal vector has components n = <nx,ny,nz> and hence the plane equation is
	// nx*(px-V0x)+ny*(py-V0y)+nz*(pz-V0z) = 0
	// If we're given px,pz as the horizontal coordinates, V0, V1 and V2 as the vertex data and have computed n then
	// the only unkown is py, which is the height value on the mesh at px,pz
	
	// SOLUTION
	// Implemented in the function InterpHeight() above
	// Solution is tested using the following coordinates, after implementing solution to Question 5 (to obtain bounding triangle vertices)

	// Question 5
	// Obtain the vertices of the triangle encompassing a given X,Z coordinate pair

	const UINT NUM_INDEX_PER_QUAD = 6;
	int	m_iTerrainSizeX=64, m_iTerrainSizeZ=64;
	float m_iTerrainMinX = -50.0f;
	float m_iTerrainMaxX =  50.0f;
	float m_iTerrainMinZ = -50.0f;
	float m_iTerrainMaxZ =  50.0f;

	float dx = px - m_iTerrainMinX;
	float dz = pz - m_iTerrainMinZ;
	const float delta = (m_iTerrainMaxX - m_iTerrainMinX) / m_iTerrainSizeX;

	UINT r = UINT(dx / delta);
	UINT c = UINT(dz / delta);

	// check that px,pz isn't on the top or right edge of the mesh
	// if it is, make sure we reference the previous row and/or col
	if (r == m_iTerrainSizeX-1)
		r -= 1;
	if (c == m_iTerrainSizeZ-1)
		c -= 1;

	// which quad is px,pz within
	UINT i = r * (m_iTerrainSizeX) + c;
	
	std::vector<VERTEX>&		vertexData = m_pLoader->GetVertexData();

	D3DXVECTOR3 V0,V1,V2;
	V0 = vertexData[i].vPosition;

	// is px,pz in the upper or lower triangle of the quad
	float delta_x = px-V0.x;
	float delta_z = pz-V0.z;

	if (delta_z > delta_x) // then in lower triangle so jump to next triangle data
	{
		V1 = vertexData[i+64].vPosition;
		V2 = vertexData[i+1+64].vPosition;
	}
	else
	{
		V1 = vertexData[i+1+64].vPosition;
		V2 = vertexData[i+64].vPosition;
	}

	// test InterpHeight using these vertices
	float py = InterpHeight(px,pz,V0,V1,V2);

	return py;

	//std::cout << "(x,z) = (" << px << "," << pz << "): y = " << py << std::endl;

	// Given an X,Z coordinate for the terrain... find the 
	// associated verices V1, V2, & V3 that it falls within

	//int row, col;

	//if( x >= m_iTerrainMinX && x <= m_iTerrainMaxX &&
	//	z >= m_iTerrainMinZ && z <= m_iTerrainMaxZ )
	//{
	//	int i;
	//	std::vector<VERTEX>&		vertexData = m_pLoader->GetVertexData();
	//	D3DXVECTOR3		p0, p1, p2, v1, v2, n;
	//	FLOAT	d;

	//	row = m_iTerrainSizeX / x;
	//	col = m_iTerrainSizeZ / z;

	//	i = row + col * m_iTerrainSizeZ;
	//	p0 = vertexData[i].vPosition;
	//	p1 = vertexData[i+1+64].vPosition;
	//	p2 = vertexData[i+64].vPosition;

	//	v1 = p1 - p0;
	//	v2 = p2 - p0;

	//	D3DXVec3Cross( &n, &v1, &v2 );
	//	D3DXVec3Normalize(&n, &n);

	//	d = D3DXVec3Dot(&n,&p0);
	//	d = d*-1.0f;

	//	return 0.0f;
	//}
	//else
	//	return 0.0f;
}

float TERRAIN::InterpHeight(float px, float pz, const D3DXVECTOR3& V0, const D3DXVECTOR3& V1, const D3DXVECTOR3& V2)
{
	// Create normal vector and temporary vector
	D3DXVECTOR3 n, n_tmp;
	// Determine unit vectors along sides of triangle using temporary variables for V1-V0 and V2-V0
	D3DXVECTOR3 u1, u2;
	D3DXVec3Normalize(&u1,&D3DXVECTOR3(V1-V0));
	D3DXVec3Normalize(&u2,&D3DXVECTOR3(V2-V0));

	// Determine the non-normalised normal vector from the cross product
	D3DXVec3Cross(&n_tmp,&u2,&u1);
	// Determine the unit normal vector
	D3DXVec3Normalize(&n,&n_tmp);

	// nx*(px-V0x)+ny*(py-V0y)+nz*(pz-V0z) = 0
	// implies py = (-nx*(px-V0x) - nz*(pz-V0z)) / ny + V0y
	// terrain mesh data should never have vertical faces (infinite gradient) so n.y != 0, but test, just in case
	if (n.y==0)
	{
		//std::cout << "Error in terrain data! Vertical face with infinite gradient detected" << std::endl;
		//std::cout << "Triangle vertices were: (" << V0 << ") (" << V1 << ") (" << V2 << ")" << std::endl;
		return 0; // Not a very good way to deal with this error. A better program would throw an exception
	}

	float py = (-n.x*(px-V0.x)-n.z*(pz-V0.z))/n.y + V0.y;

	return py;
}
