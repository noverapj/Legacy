#pragma once
#include "iodummychar.h"
class ioCannonWeaponDummy :
	public ioDummyChar
{
protected:
	enum CannonDummyState
	{
		CDS_START,
		CDS_DELAY,
		CDS_CHARGE,
		CDS_ATTACK,
		CDS_ATTACK_FAIL,
		CDS_DEADLINE,
		CDS_DESTROY,
		CDS_END,
	};

protected:
	CannonDummyState	m_CannonDummyState;
	float				m_fCheckColskipRange;

	ioHashString		m_ChargeAnimation;
	float				m_fChargeAniRate;

	ioHashString		m_AttackAnimation;
	float				m_fAttackAniRate;

	ioHashString		m_FullAttackAnimation;
	float				m_fFullAttackAniRate;

	ioHashString		m_DeadLIneAnimation;
	float				m_fDeadLIneAniRate;

	ioHashString		m_AttackFailAni;
	float				m_fAttackFailAniRate;

	DWORD				m_dwMotionEndTime;

	ioHashString		m_szCheckDummy;

	ioHashString		m_szCannonFullChargeEffect;
	bool				m_bCheckTeamAttack;
	bool m_bEnableTranslate;

protected:
	void ProcessCannonStartState();
	void ProcessCannonAttackState();
	void ProcessCannonDeadLineState();
	bool CheckLive();

public:
	void SetCannonDelayState();
	void SetCannonChargeState();
	void SetCannonAttackState( bool bCharge, DWORD& dwEndTime, DWORD& dwFireStartTime );
	DWORD SetCannonDeadLineState( DWORD dwMotionEndTime = 0 );
	void SetCannonAttackFailState( DWORD& dwEndTime );
	void AddFullChargeEffect();
	void DeleteFullChargeEffect();
	void SetTranslateEnable( bool b )		{ m_bEnableTranslate = b; }

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void Translate( const D3DXVECTOR3 &vMove );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );

public:
	virtual bool IsMountingAvailable() const;
	virtual bool IsMountAvailable() const;
	virtual bool IsNowMountEnable() const;

	virtual int  DontMoveEntityLevel() const;
	virtual void NotifyDontMoveCollisionPushingSkip( ioGameEntity* pColEntity );
	virtual bool IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const;
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );

	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	void OnDamaged( SP2Packet &rkPacket, ioBaseChar *pAttacker );
	virtual void DropZoneCheck();
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );

public:
	ioCannonWeaponDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	~ioCannonWeaponDummy(void);
};

inline ioCannonWeaponDummy* ToCannonWeaponDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_CANNON_ITEM_DUMMY )
		return NULL;

	return dynamic_cast< ioCannonWeaponDummy* >( pDummy );
}
