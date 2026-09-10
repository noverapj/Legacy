#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioRotationWeapon2 : public ioWeapon
{
public:
	enum WeaponState
	{
		WS_ROTATE,
		WS_EXPLOSION,
	};
protected:
	WeaponState m_State;
	ioEffect *m_pFireBall;
	ioOrientBox m_LocalAttackBox;;

	int m_iCreateItemIndex;
	float m_fRotationTime;

	D3DXQUATERNION m_qtStartRot;
	D3DXVECTOR3 m_vTargetPos;
	D3DXVECTOR3 m_vDiffPos;
	D3DXVECTOR3 m_vCenterPos;

	bool m_bIndependenceChar;
	bool m_bEnableDownState;
	bool m_bRemoveByWoundState;
	bool m_bRemoveBySkillEnd;

	bool m_bSetGraceClose;
	bool m_bFirst;

	ioHashString	m_stPassTargetName;

	// Explosion
	ioHashString    m_BoomEffect;
	CEncrypt<float> m_fExplosionRange;
	CEncrypt<bool>	m_bOnlyCharCollision;

	DWORD m_dwExplosionStart;
public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

	virtual void SetLiveTime( DWORD dwTime );
	virtual void SetWeaponDead();

	virtual void DestroyResource( ioPlayStage *pStage=NULL );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void CheckCurAttribute( const ioPlayEntity *pTarget );

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pEntity, ioPlayStage *pStage );
	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

public:
	void SetPassTargetName( const ioHashString& stPassName );

protected:
	virtual const WeaponAttribute* GetAttribute() const;
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	inline int GetCreateItemIndex() const { return m_iCreateItemIndex; };
	inline bool IsRemoveByWoundState() const { return m_bRemoveByWoundState; }

protected:
	void OnRotation( float fTimePerSec );
	void OnExplosion();

	void SetExplosionState( ioPlayStage *pStage );

	bool CreateFireBall( const ioHashString &szFileName );
	bool CheckOwnerState();
	bool CheakTargetState();
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

public:
	ioRotationWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioRotationWeapon2();
};

inline ioRotationWeapon2* ToRotationWeapon2( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ROTATE2 )
		return NULL;

	return dynamic_cast< ioRotationWeapon2* >( pWeapon );
}
