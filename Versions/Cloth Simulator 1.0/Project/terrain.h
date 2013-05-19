// * ************************************************************************ //
// * * General Project Header                                             * * //
// * *                                                                    * * //
// * * Disclaimer: This document represents no significant intellectual   * * //
// * * property and my be used freely. The author takes no                * * //
// * * responsibility for any loss or damage arising from the use of      * * //
// * * the computer code contained herein. Where this document is         * * //
// * * submitted as part of an assessment task this header must remain    * * //
// * * intact and the student must make clear indication which parts      * * //
// * * have been added by themselves.                                     * * //
// * *                                                                    * * //
// * ********************************************************************** * //
// * ********************************************************************** * //
// * * GFXModel.h                                                         * * //
// * * Author: Tim Wilkin                                                 * * //
// * * Created: 05/07/08     Last Modified: 30/08/10                      * * //
// * ********************************************************************** * //

#ifndef TERRAIN_H_
#define TERRAIN_H_

#include "includes.h"
#include "mesh.h"

class TERRAIN : public MESH
{
	public:
		float HeightAt(float x, float z);
		float InterpHeight(float px, float pz, const D3DXVECTOR3& V0, const D3DXVECTOR3& V1, const D3DXVECTOR3& V2);
};

#endif