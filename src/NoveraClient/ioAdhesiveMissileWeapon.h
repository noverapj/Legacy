#pragma once

#include "ioWeapon.h"

class ioEffect;

class ioAdhesiveMissileWeapon : public ioWeapon
{
public:
	enum StateControlSync
	{
		STC_NONE,
		STC_ONE_COLLISION,
		STC_MULTI_COLLISION,
		STC_TEST_MAP_COLLISION,
	};

	//collision box centerpos type
	enum CollisionBoxType
	{
		CBT_TARGET,
		CBT_WEAPON,
	};

	enum BombEffectPositionType
	{
		BEPT_TARGET,
		BEPT_WEAPON,
	};

protected:
	CollisionBoxType m_CollisionBoxType;
	BombEffectPositionType m_BombEffectPositionType;
	ioEffect *m_pFireBall;

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

	bool m_bCollisionMap;

	const WeaponAttribute *m_pSecondAttr;
	SecondAttributeList m_vSecondAttrList;

	float				m_fChangeAngleH;
	float				m_fChangeAngleV;

	//Call Attribute
	WeaponInfoList	m_CallWeaponInfoList;
	std::vector<float> m_CallWeaponRotateAngleList;

	bool m_bArleadyCallWeapon;

	bool m_bWorldAxiCallWeapon;

protected:
	void ClearData();
	void CallWeaponRotateMoveDir( D3DXVECTOR3& vOutMoveDir, D3DXVECTOR3 vCurMoveDir, float fAngle );
	void LoadCallAttributeList( ioINILoader &rkLoader );
	void CallAttribute();
public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void RotateMoveDir( float fAngleH, float fAngleV );

protected:
	bool CreateFireBall( const ioHashString &szFileName );
	void DestroyFireBall();

	void SetForceClose();
public:
	void SetSecondAttribute( SecondAttributeList vSecondList );

	float GetMoveSpeed();
	float GetMaxRange();

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

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void CheckCurAttribute( const ioPlayEntity *pTarget );

protected:
	virtual const WeaponAttribute* GetAttribute() const;
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );
	void UpdateTargetCollisionBox( D3DXVECTOR3 &vMove, bool bSetNull = false );
	void UpdateTargetCollisionBox( bool bSetNull = false );

protected:
	bool IsProtectState( ioBaseChar* pChar );
	void Explosion( ioPlayStage* pStage );
	void ExplosionNoWeaponDead( ioPlayEntity *pTarget, ioPlayStage* pStage );

public:
	ioAdhesiveMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioAdhesiveMissileWeapon();
};

inline ioAdhesiveMissileWeapon* ToAdhesiveMissileWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ADHESIVE_MISSILE )
		return NULL;

	return dynamic_cast< ioAdhesiveMissileWeapon* >( pWeapon );
}

