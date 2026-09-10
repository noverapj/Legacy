#pragma once


#include "ioWeapon.h"

class ioEntityGroup;
class ioPlayStage;
class ioOpcodeShape;

class ioSmartBombWeapon : public ioWeapon
{
protected:
	WeaponInfoList	m_CallWeaponInfoList;
	ioEffect *m_pFireMesh;

	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vRollAxis;
	ioOrientBox m_LocalAttackBox;

	ioHashString m_BoomEffect;
	
	float m_fExtentsOffSet;

	float m_fCurMoveRange;
	float m_fDownSpeed;

	float m_fWoundedFireRange;
	bool m_bWounedWaeponFire;

	float m_fExplosionRange;	
	DWORD m_dwExplosionStart;
	DWORD m_dwExplosionTime;

	DWORD m_dwCallAttrStartTime;

	float m_fPingPongPower;
	float m_fCurPingPongPower;
	int m_iMaxPingPongCnt;
	int m_iCurPingPongCnt;

	bool m_bObjectCollision;
	bool m_bCharProtectedCollision;

	bool m_bArleadyCallWeapon;

	enum ThrowState
	{
		TS_MOVE,
		TS_DOWN,
		TS_PINGPONG,
		TS_EXPLOSION_WAIT,
		TS_EXPLOSION,
	};

	ThrowState m_State;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;
	
protected:
	bool SetFireMesh( const ioHashString &szFileName );
	void SetState( ThrowState eState );
	void LoadCallAttributeList( ioINILoader &rkLoader );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );
	
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual DWORD GetCollisionEnableTime() const;
	
public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
 	virtual ActionStopType GetActionStopType() const;
 	virtual ActionStopType GetDieActionStopType() const;

	virtual bool IsCanMineExplosion();

public:
	void SetWoundedFire(bool bWounded);	
	bool IsExplosion() const;

protected:
	void OnMoveState( float fTimePerSec, ioPlayStage *pStage );
	void OnDownState( float fTimePerSec, ioPlayStage *pStage );	
	void OnPingPongState( float fTimePerSec, ioPlayStage *pStage );
	void OnExplosionState(float fTimePerSec, ioPlayStage *pStage );
	void OnWaeponDead( float fTimePerSec, ioPlayStage *pStage );
	
	void CallAttribute();

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );

public:	
	virtual void SetMoveDir( const D3DXVECTOR3 &vDir );

public:
	void SetWallCollision( const D3DXVECTOR3 &vFaceNrm );

	void RollWeapon( float fTimePerSec );
	void SetCurMoveDir( const D3DXVECTOR3 &vDir );

	float GetMaxRange();
	
public:
	ioSmartBombWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioSmartBombWeapon();
};

inline ioSmartBombWeapon* ToSmartBombWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_SMART_BOAM_WEAPON )
		return NULL;

	return dynamic_cast< ioSmartBombWeapon* >( pWeapon );
}

