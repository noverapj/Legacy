#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioMissileWeapon2 : public ioWeapon
{
public:
	enum WeaponState
	{
		WS_MOVE,
		WS_END,
	};

protected:
	WeaponState m_WeaponState;

	ioEffect *m_pFireBall;

	float	m_fCurMoveSpeed;
	float	m_fCurMoveRange;
	float	m_fCurMoveRate;

	D3DXVECTOR3 m_vStartOffSet;

	D3DXVECTOR3 m_StartPos;
	D3DXVECTOR3 m_vMoveDir;
	ChangeDirInfoList m_ChangeDirInfoList;

	bool	m_bSetGraceClose;
	bool	m_bFirst;
	bool m_bDisableOwnerScale;

	const WeaponAttribute *m_pSecondAttr;
	SecondAttributeList m_vSecondAttrList;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	void CheckChangeDir();
	void RotateMoveDir( float fAngleH, float fAngleV );
	float GetMoveSpeed();
	float GetMaxRange();

	void SetSecondAttribute( SecondAttributeList vSecondList );

	bool ChangeWeaponEffect( const ioHashString &szEffectName );

	void SetEndState( bool bSkillEnd, bool bSendNet );
	void CheckTargetColState( ioBaseChar *pChar );

protected:
	bool CreateFireBall( const ioHashString &szFileName );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetMoveSpeed( float fSpeed );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void CheckCurAttribute( const ioPlayEntity *pTarget );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

protected:
	virtual const WeaponAttribute* GetAttribute() const;
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

public:
	ioMissileWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioMissileWeapon2();
};

inline ioMissileWeapon2* ToMissileWeapon2( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_MISSILE2 )
		return NULL;

	return dynamic_cast< ioMissileWeapon2* >( pWeapon );
}

