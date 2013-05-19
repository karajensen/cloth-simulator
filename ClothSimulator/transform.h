/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Wrapper around D3DXMATRIX for easy access
*****************************************************************/

#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <functional>
#include "float3.h"

class Transform
{
public:

    typedef std::function<void(void)> UpdateFn;
    
    Transform();
    virtual ~Transform(){}

    /**
    * Set the transform matrix to the identity
    */
    void MakeIdentity();

    /**
    * Scales locally
    * @param the scale in the local x/y/z axis
    */
    void ScaleLocal(float x, float y, float z);

    /**
    * Rotates around the local x axis
    * @param the amount to rotate in radians
    */
    void RotateXLocal(float radians);

    /**
    * Rotates around the local y axis
    * @param the amount to rotate in radians
    */
    void RotateYLocal(float radians);

    /**
    * Rotates around the local z axis
    * @param the amount to rotate in radians
    */
    void RotateZLocal(float radians);

    /**
    * Adds a given vector in world coordinates to the position
    * @param the vector to translate along
    */
    void Translate(const D3DXVECTOR3& pos);

    /**
    * Translate in world coordinates along the world axis
    * @param the amount to translate along the world x/y/z axis
    */
    void Translate(float x, float y, float z);

    /**
    * Explicitly set the transform to a position
    */
    void SetPosition(float x, float y, float z);
    void SetPosition(const D3DXVECTOR3& pos);
    void SetPosition(const FLOAT3& pos);

    /**
    * Explicitly set the transform to a scale
    * Should be called before any rotation has been applied
    */
    void SetScale(float x, float y, float z);
    void SetScale(float scale);

    /**
    * Set a function to call upon update the the transform
    * These functions should not change the transform they are observing
    * @param the function to call when rotation/scale occurs
    * @param the function to call when translation occurs
    */
    void SetObserver(UpdateFn fullUpdate, UpdateFn positionalUpdate);

    /**
    * @return the transform right axis
    */
    D3DXVECTOR3 Right() const;

    /**
    * @return the transform up axis
    */
    D3DXVECTOR3 Up() const;

    /**
    * @return the transform forward axis
    */
    D3DXVECTOR3 Forward() const;

    /**
    * @return the transform position
    */
    D3DXVECTOR3 Position() const;

    /**
    * @return the current scaling value applied to the matrix
    */
    const D3DXVECTOR3& GetScaleFactor() const { return m_scaleFactor; }
    
    /**
    * Easy access matrix for the transform
    */
    D3DXMATRIX Matrix;

private:

    /**
    * Calls observer function if set
    * @param whether to call the full update funtion or not
    */
    void CallObserver(bool fullupdate);

    D3DXVECTOR3 m_scaleFactor;       ///< Current scaling value applied to the matrix
    UpdateFn m_fullUpdateFn;         ///< Function to call upon a full update
    UpdateFn m_positionalUpdateFn;   ///< Function to call upon a positional update
};