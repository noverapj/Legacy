#pragma once
class ioTargetTrace18Dummy : public ioDummyChar
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
public:
	void SetDyingState( ioBaseChar *pOwner, bool bNet );
	bool IsEnableTelepoteState();
	void TraceOwnerAttackTarget( ioBaseChar *pOwner, ioHashString szTargetName );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );

	virtual void SetDelayState( bool bNet );
	virtual void ProcessDelayState( float fTimePerSec );

public:
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );

	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );
	//virtual bool IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const;
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );

	virtual void CheckEntityCollision( ioPlayEntity *pEntity, bool bDefense );
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );

	
public:
	ioTargetTrace18Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioTargetTrace18Dummy(void);
};

inline ioTargetTrace18Dummy* ToTargetTrace18Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TARGET_TRACE18 )
		return NULL;

	return dynamic_cast< ioTargetTrace18Dummy* >( pDummy );
}