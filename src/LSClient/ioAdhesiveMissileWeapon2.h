#pragma once

#include "ioWeapon.h"
#include "IORandom.h"

class ioEffect;

class ioAdhesiveMissileWeapon2 : public ioWeapon
{
public:
	enum MissileState
	{
		MS_MOVE,
		MS_COL_MOVE,
	};

	enum StateControlSync
	{
		STC_NONE,
		STC_CHAR_COLLISION,
		STC_TEST_MAP_COLLISION,
	};

protected:
	ioEffect *m_pFireBall;
	MissileState m_MissileState;

	ioOrientBox		m_CollisionBox;
	D3DXVECTOR3		m_vExtents;
	ioHashStringVec m_szTargetsName;
	
	ioHashStringVec m_szTargetWoundBuff;
	ioHashStringVec m_szTargetCancelBuff;

	ioHashString	m_BoomEffect;

	D3DXVECTOR3 m_StartPos;
	D3DXVECTOR3 m_vMoveDir;

	CEncrypt<float>	m_fStartOffset_x;
	CEncrypt<float>	m_fStartOffset_y;
	CEncrypt<float>	m_fStartOffset_z;

	float	m_fCurMoveRange;
	float	m_fCurMoveRate;

	bool	m_bSetGraceClose;
	bool	m_bFirst;

	//
	DWORD m_dwCurRotateTime;

	DWORD m_dwMinRotateTime;
	DWORD m_dwMaxRotateTime;

	float m_fRotateStartRange;
	float m_fRotateEndRange;

	IORandom m_Random;

	//유니크장비  스트라이더 아머
	WeaponInfoList	m_CallWeaponInfoList;
	bool m_bArleadyCallWeapon;

protected:
	void ClearData();

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void RotateMoveDir( float fAngle );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	void DestroyFireBall();

	void SetForceClose();

	void ProcessMove( float fTimePerSec, ioPlayStage *pStage );

	//유니크장비  스트라이더 아머
	void LoadCallAttributeList( ioINILoader &rkLoader );
	void CallAttribute();

public:
	float GetMoveSpeed();
	float GetMaxRange();

	void SetColMoveState( ioBaseChar *pTarget, bool bSend );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

	virtual void SetWeaponIndex( DWORD dwIndex );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );
	void UpdateTargetCollisionBox( D3DXVECTOR3 &vMove, bool bSetNull = false );
	void UpdateTargetCollisionBox( bool bSetNull = false );

protected:
	bool IsProtectState( ioBaseChar* pChar );
	void Explosion( ioPlayStage* pStage );
	void ExplosionNoWeaponDead( ioPlayStage* pStage );

	void CheckOwnerRotateDir( float fTimePerSec, ioPlayStage* pStage );
	DWORD GetRotateSpeed();

	void CheckTargetRotate( ioPlayStage *pStage, float fRotAngle );

public:
	ioAdhesiveMissileWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioAdhesiveMissileWeapon2();
};

inline ioAdhesiveMissileWeapon2* ToAdhesiveMissileWeapon2( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ADHESIVE_MISSILE2 )
		return NULL;

	return dynamic_cast< ioAdhesiveMissileWeapon2* >( pWeapon );
}

