

#include "stdafx.h"
#include "ioCPU.h"
#include "ioSIMDGeneric.h"

ioSIMDGeneric::ioSIMDGeneric()
{
}

ioSIMDGeneric::~ioSIMDGeneric()
{
}

const char* ioSIMDGeneric::GetName() const
{
	return "Generic";
}

void MulJoint( D3DXVECTOR4 *pDest, const D3DXVECTOR4 *pSrc, float fWeight )
{
	pDest[0] = pSrc[0] * fWeight;
	pDest[1] = pSrc[1] * fWeight;
	pDest[2] = pSrc[2] * fWeight;
}

void MadJoint( D3DXVECTOR4 *pDest, const D3DXVECTOR4 *pSrc, float fWeight )
{
	pDest[0] += pSrc[0] * fWeight;
	pDest[1] += pSrc[1] * fWeight;
	pDest[2] += pSrc[2] * fWeight;
}

void ioSIMDGeneric::TransformVertsAndNormals( void *verts,
											  const int numVerts,
											  const D3DXVECTOR4 *joints,
											  const ioVtxNrm4 *base,
											  const JointWeight *weights )
{
	D3DXVECTOR3 *pXformVerts = (D3DXVECTOR3*)verts;
	const BYTE  *pJointsPtr = (const BYTE*)joints;

	int i=0, j=0;
	D3DXVECTOR4 vJoints[3];
	D3DXVECTOR3 vXformPos, vXformNrm;

	for( ; i<numVerts ; i++, j++ )
	{
		MulJoint( vJoints,
				  (const D3DXVECTOR4*)(pJointsPtr + weights[j].iJointMatOffset),
				  weights[j].fWeight );

		while( weights[j].iNextVertexOffset == 0 )
		{
			j++;
			MadJoint( vJoints,
					  (const D3DXVECTOR4*)(pJointsPtr + weights[j].iJointMatOffset),
				      weights[j].fWeight );
		}

		vXformPos.x = D3DXVec4Dot( vJoints + 0, &base->vPos );
		vXformPos.y = D3DXVec4Dot( vJoints + 1, &base->vPos );
		vXformPos.z = D3DXVec4Dot( vJoints + 2, &base->vPos );
		pXformVerts[0] = vXformPos;

		vXformNrm.x = D3DXVec4Dot( vJoints + 0, &base->vNrm );
		vXformNrm.y = D3DXVec4Dot( vJoints + 1, &base->vNrm );
		vXformNrm.z = D3DXVec4Dot( vJoints + 2, &base->vNrm );
		D3DXVec3Normalize( &vXformNrm, &vXformNrm );
		pXformVerts[1] = vXformNrm;

		pXformVerts+= 2;
		base++;
	}
}
























