#pragma once

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioOwnerControlDummy2 : public ioDummyChar
{
public:
	enum OwnerControlState
	{
		OCS_CONTROL,
		OCS_LOST_CONTROL,
	};

	enum OwnerControlSync
	{
		SYNC_ATTACK_KEY = 1000,
	};

protected:
	CEncrypt<DWORD> m_dwLostControlMoveTime;
	DWORD           m_dwLostControlTime;

	CEncrypt<float> m_fRotateSpeed;

	OwnerControlState m_OwnerControlState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessPos( float fTimePerSec );
	virtual void ProcessMoveState( float fTimePerSec );

protected:
	void ProcessStartState( float fTimePerSec );

protected:
	// owner_control
	void CheckKeyInputChange();
	void CheckAttackKeyControl();
	void CheckControlMove( float fTimePerSec );

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );

	void SetMoveState();
	void SetLostControl();

public:
	ioOwnerControlDummy2( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioOwnerControlDummy2();
};

inline ioOwnerControlDummy2* ToOwnerControlDummy2( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_OWNER_CONTROL2 )
		return NULL;

	return dynamic_cast< ioOwnerControlDummy2* >( pDummy );
}