#pragma once

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioMoveAttack2Dummy : public ioDummyChar
{
public:
	enum OwnerControlSync
	{
		SYNC_ATTACK_KEY    = 1000,
	};

	enum DelayState
	{
		DS_FOLLOW,
		DS_STANDBY,
	};

	enum MoveState
	{
		MS_MOVE,
		MS_RETURN,
	};

	struct TargetList
	{
		ioHashString m_TargetName;
		DWORD m_dwTargetTime;
		TargetList()
		{
			m_TargetName.Clear();
			m_dwTargetTime = 0;
		}
	};
	typedef std::vector<TargetList> vTargetList;

protected:
	DelayState m_DelayState;
	MoveState  m_MoveState;

	CEncrypt<float> m_fReturnSpeed;
	CEncrypt<bool>  m_bDisableReturnControl;

	CEncrypt<DWORD> m_dwDelayTime;
	DWORD m_dwReturnTime;

	TargetWoundType m_TargetWoundType;
	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetAngle;
	CEncrypt<float> m_fDummyHeightOffset;
	CEncrypt<DWORD> m_dwTargetRefreshTime;
	vTargetList m_vTargetList;

	ioHashString m_szTargetName;

	float m_fCheckMoveRange;

	bool m_bDefenseKey;
	bool m_bChangeKey;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessMoveState( float fTimePerSec );
	void ProcessStartState( float fTimePerSec );

	void SetDelayState( DelayState eState );
	void SetMoveState();
	void SetReturnState();

	void FindTarget();
	bool CheckTarget( const ioHashString& szTargetName );
	void SetTargetTime( const ioHashString& szTargetName );

	void CheckKeyInput();
	bool CheckOwnerBuff();

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );

	virtual bool IsActivateWeapon();

public:
	virtual void CheckCollisionWeapon( ioBaseChar *pChar );

public:
	ioMoveAttack2Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioMoveAttack2Dummy();
};

inline ioMoveAttack2Dummy* ToioMoveAttack2Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_MOVE_ATTACK2 )
		return NULL;

	return dynamic_cast< ioMoveAttack2Dummy* >( pDummy );
}