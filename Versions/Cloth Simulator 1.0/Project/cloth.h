
#ifndef CLOTH_H_
#define CLOTH_H_


#include "includes.h"
#include "shader.h"
#include "particle.h"
#include "collision.h"

class CLOTH
{
private:
	vector<PARTICLE> particles;
	vector<SPRING> springs;
	vector<int> selected;
	vector<COLLISION_SPHERE> vertices;

	int SpringCount;
	int SpringIterations;
	int VertLength;
	int VertWidth;
	int VertCount;
	bool Simulate;
	bool Manipulate;
	bool SelfCollide;

	bool DrawMesh;
	bool DrawVerts;
	D3DXMATRIX worldMatrix;
	SHADER * m_shader;
	LPDIRECT3DTEXTURE9	m_texture;
	LPD3DXMESH	m_pMesh;

	vector<VERTEX> vertexData;
	vector<DWORD> indexData;
	void* pvVoid;
	void* piVoid;

public:

	CLOTH(){ m_pMesh = NULL; m_texture = NULL; }
	void OnTimeStep();
	void Reset();
	void ToggleSimulation();
	void UpdateVertexBuffer();
	void UpdateNormals();
	void OnDraw();
	void CreateVertexPoints();
	bool OnLoad(int width, int length, float scale, SHADER * meshFX);
	void AddTexture(char * texDiffuse);
	void AddForce(FLOAT3 & force);
	void ReleaseCloth();

	void SetSelfCollide(bool col){SelfCollide = col;}
	bool SelfCollisionTest();
	bool IsSelfColliding(){return SelfCollide;}
	void TestSphereCollision(COLLISION_SPHERE * sphere);
	void TestBoxCollision(COLLISION_BOX * box);

	void SetManipulate(bool manip){Manipulate = manip;}
	bool IsManipulating(){return Manipulate;}
	void SetSimulate(bool sim){Simulate = sim;}
	bool IsSimulating(){return Simulate;}
	unsigned int getVertCount(){return VertCount;}
	int GetSpringIterations(){return SpringIterations;}
	void SetSpringIterations(int it){SpringIterations = it;}
	PARTICLE* getParticle(int x, int y) {return &particles[y*VertWidth + x];}
	VERTEX* getVertex(int x, int y) {return &vertexData[y*VertWidth + x];}
	D3DXVECTOR3 CalculateTriNormal(PARTICLE * p1, PARTICLE * p2, PARTICLE * p3);

	//vertex manipulation
	int SelectedRow;
	void ToggleShowVerts();
	void MousePickingTest();
	void UnpinCloth();
	void SelectRow(int row);
	void DeselectRow();
	void MovePinnedRowUp(float speed);
	void MovePinnedRowRight(float speed);
	void MovePinnedRowForward(float speed);


};

#endif