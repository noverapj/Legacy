
#pragma once

#include "ioWeapon.h"

class ioEffect;

class ioGroundTraceWeapon3 : public ioWeapon
{
protected:
	enum GroundTraceState
	{
		GTS_MOVE_AIR,
		GTS_MOVE_LAND,
		GTS_TRACE,
	};

protected:
	float m_fStartSpeed;
	float m_fMoveSpeed;
	float m_fTraceSpeed;

	float m_fEnableMoveGap;
	float m_fDefaultGapHeight;

	ChangeDirInfoList m_ChangeDirInfoList;		
	WeaponInfoList m_CallWeaponInfoList;

	float m_fSensingRange;
	float m_fSensingAngle;
	DWORD m_dwTraceTime;

protected:
	GroundTraceState m_GroundTraceState;

	ioEffect *m_pWeaponMesh;
	ioOrientBox m_LocalAttackBox;

	D3DXVECTOR3 m_vMoveDir;
	float m_fCurMoveSpeed;
	float m_fCurMoveRange;

	bool m_bArleadyCallWeapon;

	ioHashString m_szTargetName;
	DWORD m_dwCheckTraceTime;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;
	
public:
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

public:
	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage = NULL );
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual void SetOwnerChargeScaleRate( float fRate );
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision = false );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );
	
protected:
	bool SetMesh( const ioHashString &szFileName );

protected:
	void LoadCallAttributeList( ioINILoader &rkLoader );
	void CallAttribute();

protected:
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );
	float GetMoveSpeed();
	float GetMaxRange();

public:
	void SetTraceState( ioPlayStage *pStage, bool bSendNetwork );
	void SetTraceTarget( ioHashString &szName );

protected:
	void OnMoveAir( float fTimePerSec, ioPlayStage *pStage );
	void OnMoveLand( float fTimePerSec, ioPlayStage *pStage );
	void OnTraceLand( float fTimePerSec, ioPlayStage *pStage );

protected:
	bool IsEnableTarget( ioBaseChar *pTarget );
	
public:
	ioGroundTraceWeapon3( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioGroundTraceWeapon3();
};

inline ioGroundTraceWeapon3* ToGroundTraceWeapon3( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_GROUND_TRACE3 )
		return NULL;

	return dynamic_cast< ioGroundTraceWeapon3* >( pWeapon );
}

