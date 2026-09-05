

#ifndef _ioMeshTrailer_h_
#define _ioMeshTrailer_h_

#include "ioRenderable.h"
#include "ioCurveGenerator.h"

class ioCamera;
class ioMaterial;
class ioRenderOperation;
class ioEntityGroup;
class ioMeshData;
class ioMeshControlPoint;

class MeshTrailData;
class ioAnimation;
class ioPass;

class __EX ioMeshTrailer : public ioRenderable
{
	friend ioEntityGroup;

protected:
	ioEntityGroup *m_pOwner;
	ioMeshData *m_pMeshData;
	ioMaterial *m_pMtl;

	ioHashString m_TextureName;
	FXRenderType m_RenderType;

	ioHashString m_StartLinkBiped;
	ioHashString m_EndLinkBiped;

	D3DXVECTOR3 m_vStartPoint;
	D3DXVECTOR3 m_vEndPoint;

	D3DXVECTOR3 m_vCurStartPoint;
	D3DXVECTOR3 m_vCurEndPoint;

	Vector3Vec m_vCtrlPointList;
	ioNoneUniformSpline m_GenStart;
	ioNoneUniformSpline m_GenEnd;

	int m_iMaxTickSegments;
	float m_fMaxTickTime;

	float m_fGlowness;
	float m_fIntensity;

	MeshTrailData *m_pTrailData;
	const ioAnimation *m_pTrailAni;
	float m_fTrailStartTime;
	float m_fAniTimeFactor;

	float m_fGraseCloseSetTime;
	float m_fGraceCloseAniTime;
	bool m_bGraceClose;

	int  m_iCurVertexIndex;
	bool m_bUpdateBufferOK;

	bool  m_bMaintain;
	float m_fMaintainTime;
	float m_fMaintainDepth;
	D3DXMATRIX m_matMaintainTransform;
	float m_fMaintainStartPoint;
	float m_fMaintainEndPoint;
	float m_fMaintainGapPoint;

protected:
	static float m_fTrailSegmentRate;
	static bool  m_bUseGlowTrail;

public:
	void SetTexture( const ioHashString &szName );
	void SetRenderType( FXRenderType eType );
	void SetGraceClose();

	void SetMaxTickTime( float fTime );
	void SetMaxSegment( int iMaxSegment );
	void SetDepthCheck( bool bDepthCheck );

	void SetGlowness( float fGlow );
	void SetIntensity( float fIntensity );

	void SetStartEndPoint( const ioMeshControlPoint *pStart,
						   const ioMeshControlPoint *pEnd );

	bool GenerateTrailData( const ioHashString &rkSkeleton,
							const ioHashString &rkMeshName,
						    const ioAnimation *pTrailAni,
						    int iCtrlPointIdx,
						    float fSampleTick,
						    float fEventStartTime,
							float fMaintainTime = 0.0f );

	void BuildMaterial();

protected:
	void SetPassRenderType( ioPass *pPass, FXRenderType eType );
	void BuildNormalMaterial();
	void BuildGlowMaterial();

public:
	bool UpdateCtrlPoints( float fTime );
	void NotifyCurrentCamera( ioCamera *pCamera );

protected:
	bool CreateBuffers();
	void FillBuffers( void *pBuf, int iCurMaxSegment );

public:
	const ioHashString& GetTrailAniName() const;
	inline const ioHashString& GetStartBiped() const { return m_StartLinkBiped; }
	inline const ioHashString& GetEndBiped() const { return m_EndLinkBiped; }

	inline const D3DXVECTOR3& GetStartPoint() const { return m_vStartPoint; }
	inline const D3DXVECTOR3& GetEndPoint() const { return m_vEndPoint; }

	inline bool IsUpdateBufferOK() const { return m_bUpdateBufferOK; }

public:
	virtual ioMaterial* GetMaterial() const;
	virtual float GetSquaredViewDepth( const ioCamera *pCamera );
	virtual void GetRenderOperation( ioRenderOperation &rkRenderOp );
	virtual void GetWorldTransform( D3DXMATRIX *pMatWorld ) const;

public:
	static bool SetUseGlowTrail( bool bUse );
	static bool UseGlowTrail() { return m_bUseGlowTrail; }

	// 0.3f ~ 1.0f
	static void SetTrailSegmentRate( float fRate );

protected:
	ioMeshTrailer( ioEntityGroup *pGrp );
	virtual ~ioMeshTrailer();
};

#endif