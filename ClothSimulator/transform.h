////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - transform.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <functional>
#include "matrix.h"

/**
* Matrix with rotation and scale components
*/
class Transform : public Matrix
{
public:

    typedef std::function<void(void)> UpdateFn;
    
    Transform();
    virtual ~Transform(){}

    /**
    * Set the transform matrix to the identity
    */
    virtual void MakeIdentity() override;

    /**
    * Adds a given vector in local coordinates to the position
    * @param position The vector to translate along
    */
    void Translate(D3DXVECTOR3 position);

    /**
    * Adds a given vector in world coordinates to the position
    * @param position The vector to translate along
    */
    void TranslateGlobal(D3DXVECTOR3 position);

    /**
    * Adds the components in local coordinates to the position
    * @param x/y/z The components to translate along
    */
    void Translate(float x, float y, float z);

    /**
    * Explicitly set the matrix to a position
    * @param x/y/z The components to set as the position
    */
    virtual void SetPosition(float x, float y, float z) override;

    /**
    * Explicitly set the matrix to a position
    * @param position The vector to set as the position
    */
    virtual void SetPosition(const D3DXVECTOR3& position) override;

    /**
    * Scales the matrix using local axis
    * @param x/y/z The amount to scale each component
    */
    void Scale(float x, float y, float z);

    /**
    * Rotates the matrix around an axis
    * @param radians The angle in radians to rotate around
    * @param axis The axis to rotate around
    */
    void RotateAroundAxis(float radians, D3DXVECTOR3 axis);

    /**
    * Explicitly set the transform to a scale
    * Should be called before any rotation has been applied
    * @param x/y/z The components to set as the scale
    */
    void SetScale(float x, float y, float z);

    /**
    * Explicitly set the transform to a scale
    * Should be called before any rotation has been applied
    * @param scale The scale to be set for each components
    */
    void SetScale(const D3DXVECTOR3& scale);

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
    * @return the current scaling value applied to the matrix
    */
    D3DXVECTOR3 GetScale() const;

    /**
    * @return the rotation matrix
    */
    const D3DXMATRIX& GetRotationMatrix() const { return m_rotation; }

    /**
    * Sets the rotation matrix
    */
    void SetRotationMatrix(const D3DXMATRIX& mat) { m_rotation = mat; }

private:

    /**
    * Calls observer function if set and updates the transform
    * @param fullupdate Whether to call the full update funtion or not
    */
    void Update(bool fullupdate);

    float m_yaw;                     ///< Radian amount of yaw
    float m_pitch;                   ///< Radian amount of pitch
    float m_roll;                    ///< Radian amount of roll
    D3DXVECTOR3 m_position;          ///< Position for the transform
    D3DXMATRIX m_rotation;           ///< Current rotation applied to the matrix
    D3DXMATRIX m_scale;              ///< Current scaling applied to the matrix
    UpdateFn m_fullUpdateFn;         ///< Function to call upon a full update
    UpdateFn m_positionalUpdateFn;   ///< Function to call upon a positional update
};