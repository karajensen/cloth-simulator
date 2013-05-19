
#ifndef PARTICLE_H_
#define PARTICLE_H_


#include "includes.h"

static const float PARTICLE_MASS = 1.0;


//PARTICLE CLASS
class PARTICLE
{
private:
	bool pinned;
	FLOAT3 acceleration;

public:
	FLOAT3 old_position;
	FLOAT3 initial_position;
	FLOAT3 position;

	PARTICLE(){};
	void InitialiseParticle();
	void AddForce(FLOAT3 & force);
	void PinParticle(bool pin){pinned = pin;};
	bool IsPinned(){return pinned;};
	void MovePosition(const FLOAT3 & v){if(!pinned){position += v;}};
	void OnTimeStep();
	void ResetAcceleration(){acceleration.SetToZero();}
};

//SPRING CLASS
class SPRING
{
private:
	float restDistance; //distance for spring at rest
public:
	PARTICLE *p1,*p2; //connected particles
	SPRING(PARTICLE*pt1,PARTICLE *pt2);
	void SolveSpring();
};


#endif