

#ifndef _ioSceneShadowBox_h_
#define _ioSceneShadowBox_h_

#include "io3DCommon.h"

#include "ioPlane.h"
#include "ioAxisAlignBox.h"
#include "ioPolygonMesh.h"

class ioCamera;
class ioMovableObject;

class __EX ioSceneShadowBox
{
private:
	ioCamera	*m_pCamera;
	D3DXVECTOR3	m_vLightDir;

	ioAxisAlignBox m_SceneBox;
	float m_fClosingOffset;
	float m_fExtendDiagonalRate;

	typedef std::vector< ioPlane > PlaneList;

	ioPlane::Side *m_aOutSidePolygon;
	int m_iCurOutSideArraySize;

public:
	bool AddShadowCaster( ioMovableObject *pObj, float fScale = 1.0f );
	void AddShadowReceiveBox( const ioAxisAlignBox &kAxisBox, float fScale = 1.0f );
	void CreateShadowFrustum( Vector3Vec &kVolumnVtxList );

	// 전체 씬의 영역에 따라 MakeClosing에서 사용될 offset값을 조정할 필요가 있음
	void SetMakeClosingPolygonOffset( float fOffset );
	void SetExtendDiagonalRate( float fRate );

private:
	void ClipFrustomByPlaneList( const PlaneList &rkPlaneList, ioPolygonMesh &kFrustum );

	void ClipPolygonByPlane( const ioPolygon &rkSrcPoly,
							 const ioPlane &kPlane,
							 ioPolygon &kOutPoly,
							 ioPolygon &kInterPoly );


	void MakeViewFrustumPolygon( ioPolygonMesh &kFrustum );

	void CalcAABoxPlaneList( const ioAxisAlignBox &kSceneBox, PlaneList &kPlaneList );
	void CheckOutSidePolygonArray( int iNeedArray );
	
private:
	void MakeClosingPolygon( ioPolygonMesh &kSrcMesh,
							 ioPolygonMesh &kOutMesh,
							 const D3DXVECTOR3 &vPlaneNrm );

	int FindSamePointInMeshAndSwapWithLast( ioPolygonMesh &kPolyMesh, const D3DXVECTOR3 &vLastPt );
	int FindSamePointInPolygon( const ioPolygon &rkPoly, const D3DXVECTOR3 &vLastPt );

public:
	bool IsShadowBoxEmpty() const;

public:
	ioSceneShadowBox( ioCamera *pCamera, const D3DXVECTOR3 &vLightDir );
	~ioSceneShadowBox();
};

#endif