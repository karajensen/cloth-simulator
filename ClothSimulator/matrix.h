////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - matrix.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <d3d9.h>
#include <d3dx9.h>

/**
* Basic D3DXMATRIX matrix with read/write properties
*/
class Matrix
{
public:

    Matrix();
    virtual ~Matrix(){}

    /**
    * Set the matrix to the identity
    */
    virtual void MakeIdentity();

    /**
    * Explicitly set the matrix to a position
    * @param x/y/z The components to set as the position
    */
    virtual void SetPosition(float x, float y, float z);

    /**
    * Explicitly set the matrix to a position
    * @param position The vector to set as the position
    */
    virtual void SetPosition(const D3DXVECTOR3& position);

    /**
    * Explicitly set the matrix axis
    * @param up The up vector
    * @param forward The forward vector
    * @param right The right vector
    */
    void SetAxis(const D3DXVECTOR3& up, const D3DXVECTOR3& forward, const D3DXVECTOR3& right);

    /**
    * @return the matrix right axis
    */
    D3DXVECTOR3 Right() const;

    /**
    * @return the matrix up axis
    */
    D3DXVECTOR3 Up() const;

    /**
    * @return the matrix forward axis
    */
    D3DXVECTOR3 Forward() const;

    /**
    * @return the matrix position
    */
    D3DXVECTOR3 Position() const;

    /**
    * @return the const matrix for the transform
    */
    const D3DXMATRIX& GetMatrix() const { return m_matrix; }

    /**
    * @return the matrix pointer for the transform
    */
    D3DXMATRIX* MatrixPtr() { return &m_matrix; }

    /**
    * Matrix Equality
    * @param matrix The matrix to make equal to
    */
    void Set(const D3DXMATRIX& matrix);

    /**
    * Matrix multiplication for a matrix without scaling
    * @param matrix The matrix to multiply with
    */
    void Multiply(const D3DXMATRIX& matrix);

protected:

    D3DXMATRIX m_matrix; ///< Internal directX matrix

};
