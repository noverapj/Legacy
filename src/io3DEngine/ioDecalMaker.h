

#ifndef _ioDecalMaker_h_
#define _ioDecalMaker_h_

#include "ioPlane.h"

class ioOrientBox;
class ioDecal;
class ioEntity;

class __EX ioDecalMaker
{
protected:
	typedef std::vector< ioPlane > PlaneList;
	PlaneList m_CullPlaneList;

	D3DXVECTOR3 m_vCenter;
	D3DXVECTOR3 m_vNormal;
	D3DXVECTOR3 m_vTangent;
	D3DXVECTOR3 m_vBinormal;

	float m_fWidth;
	float m_fHeight;
	float m_fBackFaceEpsilon;

	Vector3Vec m_vVtxList;
	Vector2Vec m_vUVList;
	WordVec m_vIdxList;

	bool m_bRecalcMinMax;

public:
	void Initialize( const ioOrientBox &rkBound, float fEpsilon );

	void AddDecal( ioEntity *pEntity );
	ioDecal* BuildDecal( const ioHashString &rkName, int iDivLv = 0 );

protected:
	void AddDecal( int iFaceCnt, const WORD *pIdx, const D3DXVECTOR3 *pVtx );

	int ClipPolygon( int iVtxCnt, D3DXVECTOR3 *pNewVtx );

	int ClipPolygonByPlane( const ioPlane &rkPlane,
							int iVtxCnt,
							const D3DXVECTOR3 *pVtx,
							D3DXVECTOR3 *pNewVtx );

	void AddPolygon( int iVtxCnt, const D3DXVECTOR3 *pVtx );

protected:
	void GenerateAutoUV();

public:
	ioDecalMaker();
	virtual ~ioDecalMaker();
};

#endif
