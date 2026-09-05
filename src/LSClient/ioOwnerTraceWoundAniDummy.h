#pragma once
#include "iodummychar.h"

class ioOwnerTraceWoundAniDummy :	public ioDummyChar
{
private:
	enum DummyState
	{
		DS_START,
		DS_DELAY,
		DS_OWNER_TRACE,
		DS_TARGET_SENSING,
		DS_TARGET_TRACE,
		DS_TARGET_ATTACK,
		DS_WOUNDED,
		DS_DYING,
		DS_DIE,
	};

	enum TargetTraceSync
	{
		TTS_OWNER_TRACE		= 1000,
		TTS_SENSING,
		TTS_TARGET_TRACE,
		TTS_TRACE_ATTACK,
		TTS_OWNER_ATTACK_TARGET,
		TTS_DYING,
	};

	enum OwnerControlSync
	{
		SYNC_APPLY_WEAPON = 2000,
	};
private:
	DummyState		m_DummyState;

	//Area Weapon
	ioHashString	m_AreaWeaponName;
	int				m_iAreaWeaponIndex;
	D3DXVECTOR3		m_vAreaWeaponPos;
	bool			m_bAttachAreaWeapon;

	//Owner Trace
	ioHashString	m_OwnerTraceAni;
	float			m_fOwnerTraceAniRate;
	float			m_fOwnerTraceCheckRange;
	float			m_fOwnerTraceRange;
	float			m_fOwnerTraceSpeed;
	D3DXVECTOR3		m_vOwnerTracingMoveDir;
	bool			m_bNoDropZoneMove;

	ioHashString	m_szZoneAttachEffect;		//Owner만 보이는 Effect

	//Attack
	int					m_iCurAttackCount;
	std::vector<int>	m_vAttackCode;

	//End Ani
	ioHashString	m_szDyingAnimation;
	float			m_fDyingAniRate;
	DWORD			m_dwDyingEndTime;

	//HP 흡수
	ioHashString	m_szHPAbsorbBuff;
	float			m_fAbsorbGauge;

	ioHashString	m_szOwnerAttackTargetName;

	ioHashString	m_szOwnerTraceRangeEffect;

	bool			m_bTargetRotAttack;

	// last attack
	DWORD	m_dwLastApplyWeaponIdx;
	DWORD	m_dwLastApplyWeaponTime;

	// apply weapon
	AttackSide	m_AttackedSide;
	AniDuration	m_AnimationTime;

	// wound animation
	ioHashString	m_szWoundAni;
	float	m_fWoundAniRate;

	// special attack
	bool	m_bSpecialAttack;
	DummyAttackInfo	m_DummySpecialAttack;

	// owner effect
	ioHashString m_OwnerEffect;

	// drop zone move
	bool	m_bDropZoneFlyMove;
	
private:
	void SetLoopAni( ioHashString szAniName, float fRate = 1.0f );

	void ProcessStartState();

	bool CheckOwnerTrace( ioBaseChar *pOwner );
	void SetOwnerTraceState( ioBaseChar *pOwner );
	void ProcessOwnerTraceState( ioBaseChar *pOwner, float fTimerPerSec );

	void SetSensingState( ioBaseChar *pOwner, bool bNet );
	void ProcessSensingState( ioBaseChar *pOwner, float fTimerPerSec );

	void SetTargetTraceState( ioBaseChar *pOwner );
	void ProcessTargetTraceState( ioBaseChar *pOwner, float fTimerPerSec );
	bool IsTraceTarget( ioHashString szTargetName, ioPlayStage *pStage );

	void CheckTraceAttack( ioBaseChar *pOwner );
	void SetTracingAttack( ioBaseChar *pOwner, bool bNet );
	void ProcessTracingAttack( ioBaseChar *pOwner, float fTimePerSec, DWORD dwPreTime );

	void ProcessDyingState( ioBaseChar *pOwner );

	void CheckTracingDir( float fTimerPerSec, D3DXVECTOR3& vTracingMoveDir, D3DXVECTOR3 vDiff );
	void ProcessTraceMoving( ioBaseChar *pOwner, D3DXVECTOR3& vDir, float fMoveAmt, bool bNoDropZoneMove );
	void ProcessAreaWeapon();

	void CheckDyingByHp();

	bool CheckOwnerDestroyState();
public:
	void SetDyingState( ioBaseChar *pOwner, bool bNet );
	bool IsEnableTelepoteState();
	void TraceOwnerAttackTarget( ioBaseChar *pOwner, ioHashString szTargetName );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void InitDummyCharInfo(	int iIndex,
		DWORD dwCreateGapTime,
		const D3DXVECTOR3 &vPos,
		const D3DXVECTOR3 &vScale,
		float fStartAngle=0.0f, bool bNetWork = false );

protected:
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessPos( float fTimePerSec );

	virtual void SetDelayState( bool bNet );
	virtual void ProcessDelayState( float fTimePerSec );

public:
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );

	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );

	virtual void CheckEntityCollision( ioPlayEntity *pEntity, bool bDefense );
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	AttackSide GetAttackedSide( const D3DXVECTOR3 &vAttackDir ) const;
	void SetLastApplyWeapon( int iIndex, DWORD dwTime );
	void SetWoundedState( const ioHashString &szWoundedAni, float fTimeRate );
	void ProcessWoundedState();
	void ApplyWeaponWounded( ioWeapon *pWeapon, AttackSide eSide, const D3DXVECTOR3 &vAttackDir );
	void ApplyWeaponWoundedInNetwork( ioPlayStage *pStage, SP2Packet &rkPacket );

public:
	ioOwnerTraceWoundAniDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual	~ioOwnerTraceWoundAniDummy(void);
};

inline	ioOwnerTraceWoundAniDummy*	ToOwnerTraceWoundAniDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_OWNER_TRACE_WOUND_ANI )
		return	NULL;

	return	dynamic_cast< ioOwnerTraceWoundAniDummy* >( pDummy );
}