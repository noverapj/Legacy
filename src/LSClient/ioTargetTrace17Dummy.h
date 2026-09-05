#pragma once
class ioTargetTrace17Dummy : public ioDummyChar
{
private:
	enum TraceDummyState
	{
		TDS_START,
		TDS_PRE_DELAY,
		TDS_DELAY,
		TDS_MOVE,
		TDS_ATTACK,
	};

	enum DummyCharSyncType
	{
		DCST_MOVE		= 1000,
		DCST_DELAY		= 1001,
		DCST_ATTACK		= 1002,
	};

private:
	TraceDummyState		m_TraceDummyState;
	DWORD				m_dwStateEndTime;
private:
	//Pre
	DWORD				m_dwPreDelayDuration;
	ioHashString		m_PreDelayAnimation;
	float				m_fPreDelayAniRate;

	//Delay
	DWORD				m_dwDelayDurationTime;

	//Move
	DWORD				m_dwMoveDuration;
	float				m_fCurMoveRange;
	float				m_fMaxMoveRange;
	float				m_fDummyMoveSpeed;
	D3DXVECTOR3			m_vMoveDir;

	//Attack
	DWORD				m_dwAttackCode;
	int					m_iMaxMoveCount;
	int					m_iCurMoveCount;

	ioHashStringVec		m_vTargetBuffList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessPos( float fTimePerSec );
	virtual void ProcessAttackState( float fTimePerSec, DWORD dwPreTime );

	virtual void CheckEntityCollision( ioPlayEntity *pEntity, bool bDefense );
public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
private:
	void Init();
	void ProcessStartState( ioBaseChar *pOwner );
	void ProcessPreDelayState( ioBaseChar *pOwner );
	void ProcessMoveState( ioBaseChar *pOwner );
	void ProcessDelayState( ioBaseChar *pOwner );

	void SetLoopAni( ioBaseChar *pOwner, const ioHashString& szName, float fAniRate );
	void SetPreDelayState( ioBaseChar *pOwner );
	void SetMoveState( ioBaseChar *pOwner, const ioHashString& szTarget, bool bUseNoneTargetMove );
	void SetDelayState( ioBaseChar *pOwner );
	void SetAttackState( ioBaseChar *pOwner, ioHashString szTargetName );
	void SetAttackAttribute( ioBaseChar *pOwner );
	void AddTargetBuff( ioBaseChar *pOwner, ioBaseChar *pTarget );

	bool CheckMove( ioBaseChar *pChar, ioPlayStage *pStage );
	void DestroyAttachWeaponIndexList( ioBaseChar *pChar );

public:
	ioTargetTrace17Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	~ioTargetTrace17Dummy(void);
};

inline ioTargetTrace17Dummy* ToTargetTrace17Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TARGET_TRACE17 )
		return NULL;

	return dynamic_cast< ioTargetTrace17Dummy* >( pDummy );
}