#ifndef FLOATTHREE_H_
#define FLOATTHREE_H_



class FLOAT3
{
public:

	float x,y,z;
	FLOAT3(float X = 0, float Y = 0, float Z = 0): x(X),y(Y),z(Z){};

	//MAKE NEGATIVE
	FLOAT3 operator-()
	{
		return FLOAT3(-x,-y,-z);
	}
	//TIMES BY SCALAR
	FLOAT3 operator*(float a)
	{
		FLOAT3 newVector;
		newVector.x = a*x;
		newVector.y = a*y;
		newVector.z = a*z;
		return newVector;
	}
	//TIMES BY SCALAR
	void operator*=(float a)
	{
		x = x*a;
		y = y*a;
		z = z*a;
	}
	//DIVIDE BY SCALAR
	FLOAT3 operator/(float a)
	{
		FLOAT3 newVector;
		newVector.x = x/a;
		newVector.y = y/a;
		newVector.z = z/a;
		return newVector;
	}
	//DIVIDE BY SCALAR
	void operator/=(float a)
	{
		x = x/a;
		y = y/a;
		z = z/a;
	}
	//ADD TWO VECTORS
	FLOAT3 operator+(const FLOAT3 & v)
	{
		FLOAT3 newVector;
		newVector.x = x + v.x;
		newVector.y = y + v.y;
		newVector.z = z + v.z;
		return newVector;
	}
	//ADD TWO VECTORS
	void operator+=(const FLOAT3 & v)
	{
		x = x + v.x;
		y = y + v.y;
		z = z + v.z;
	}
	//MINUS TWO VECTORS
	FLOAT3 operator-(const FLOAT3 & v)
	{
		FLOAT3 newVector;
		newVector.x = x - v.x;
		newVector.y = y - v.y;
		newVector.z = z - v.z;
		return newVector;
	}
	//MINUS TWO VECTORS
	void operator-=(const FLOAT3 & v)
	{
		x = x - v.x;
		y = y - v.y;
		z = z - v.z;
	}
	//CROSS PRODUCT
	FLOAT3 Cross(const FLOAT3 & v)
	{
		FLOAT3 crossed;
		crossed.x = (y*v.z)-(z*v.y);
		crossed.y = -((x*v.z)-(z*v.x));
		crossed.z = (x*v.y)-(y*v.x);
		return crossed;
	}
	//DOT PRODUCT
	float Dot(const FLOAT3 & v)
	{
		return ((v.x*x)+(v.y*y)+(v.z*z));
	}
	//LENGTH OF VECTOR
	float Length()
	{
		return sqrt((x*x)+(y*y)+(z*z));
	}
	//CHANGE VECTOR
	void Change(FLOAT3 & v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
	//NORMALIZE VECTOR
	FLOAT3 & Normalize()
	{
		float length = this->Length();
		x = x/length;
		y = y/length;
		z = z/length;
		return *this;
	}
	//GET NORMAL
	FLOAT3 GetNormalized()
	{
		float length = this->Length();
		FLOAT3 newVec;
		newVec.x = x/length;
		newVec.y = y/length;
		newVec.z = z/length;
		return newVec;
	}
	void SetToZero()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}
};
#endif
