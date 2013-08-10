////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <functional>

/**
* Wrapper around D3DXMATRIX for easy access
*/
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
    * Scales the matrix using local axis
    * @param x/y/z The amount to scale each component
    */
    void ScaleLocal(float x, float y, float z);

    /**
    * Rotates the matrix
    * @param radians The amount to rotate in radians
    * @param local Whether to rotate around the local x axis
    */
    void RotateX(float radians, bool local);

    /**
    * Rotates the matrix
    * @param randians The amount to rotate in radians
    * @param local Whether to rotate around the local y axis
    */
    void RotateY(float radians, bool local);

    /**
    * Rotates the matrix 
    * @param radians The amount to rotate in radians
    * @param local Whether to rotate around the local z axis
    */
    void RotateZ(float radians, bool local);

    /**
    * Adds a given vector in world coordinates to the position
    * @param position The vector to translate along
    */
    void Translate(const D3DXVECTOR3& position);

    /**
    * Adds the components in world coordinates to the position
    * @param x/y/z The components to translate along
    */
    void Translate(float x, float y, float z);

    /**
    * Explicitly set the transform to a position
    * @param x/y/z The components to set as the position
    */
    void SetPosition(float x, float y, float z);

    /**
    * Explicitly set the transform to a position
    * @param position The vector to set as the position
    */
    void SetPosition(const D3DXVECTOR3& position);

    /**
    * Explicitly set the transform to a scale
    * Should be called before any rotation has been applied
    * @param x/y/z The components to set as the scale
    */
    void SetScale(float x, float y, float z);

    /**
    * Explicitly set the transform to a scale
    * Should be called before any rotation has been applied
    * @param scale The scale to be set to all components
    */
    void SetScale(float scale);

    /**
    * Set a function to call upon update the the transform
    * These functions should not change the transform they are observing
    * @param fullUpdate The function to call when rotation/scale occurs
    * @param positionalUpdate The function to call when translation occurs
    */
    void SetObserver(UpdateFn fullUpdate, UpdateFn positionalUpdate);

    /**
    * Explicitly set the transform axis
    * @param up The up vector
    * @param forward The forward vector
    * @param right The right vector
    */
    void SetAxis(const D3DXVECTOR3& up, const D3DXVECTOR3& forward, const D3DXVECTOR3& right);

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
    * @return the const matrix for the transform
    */
    const D3DXMATRIX& Matrix() const { return m_matrix; }

    /**
    * @return the matrix pointer for the transform
    */
    D3DXMATRIX* MatrixPtr() { return &m_matrix; }

    /**
    * Matrix Equality
    * @param matrix The matrix to make equal to
    * @param scalefactor The scalefactor to make equal to
    */
    void Equals(const D3DXMATRIX& matrix, const D3DXVECTOR3& scalefactor);

    /**
    * Matrix multiplication for a matrix without scaling
    * @param matrix The matrix to multiply with
    */
    void Multiply(const D3DXMATRIX& matrix);

private:

    /**
    * Calls observer function if set
    * @param fullupdate Whether to call the full update funtion or not
    */
    void CallObserver(bool fullupdate);

    D3DXMATRIX m_matrix;             ///< Matrix for the transform
    D3DXVECTOR3 m_scaleFactor;       ///< Current scaling value applied to the matrix
    UpdateFn m_fullUpdateFn;         ///< Function to call upon a full update
    UpdateFn m_positionalUpdateFn;   ///< Function to call upon a positional update
};