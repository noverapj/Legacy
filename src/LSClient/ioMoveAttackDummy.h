#pragma once

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioMoveAttackDummy : public ioDummyChar
{
public:
	enum AimType
	{
		AT_AIM = 0,
		AT_ZERO_AIM = 1,
	};

	enum OwnerControlSync
	{
		SYNC_ATTACK_KEY    = 1000,
		SYNC_CHANGE_TARGET = 1001,
	};

	struct AimInfo
	{
		CEncrypt<DWORD> m_dwAimTime;
		ioHashString m_AimEffect;
		DWORD m_dwAimEffectID;
		
		AimInfo()
		{
			m_dwAimTime = 0;
			m_dwAimEffectID = 0;
			m_AimEffect.Clear();
		}
	};

protected:
	CEncrypt<float> m_fAimMoveSpeed;
	CEncrypt<float> m_fZeroAimMoveSpeed;

	TargetWoundType m_TargetWoundType;
	CEncrypt<float> m_fAimRange;

	WeaponInfo m_AimWeapon;
	WeaponInfo m_ZeroAimWeapon;

	AimType m_CurAimType;
	AimInfo m_AimInfo[2];

	DWORD m_dwAimCheckTime;

	CEncrypt<int> m_iMaxMoveCount;
	CEncrypt<int> m_iCurMoveCount;
	
	BaseCharDistanceSqList m_vTargetList;
	ioHashString m_szTargetName;

	float m_fCheckMoveRange;

	CEncrypt<bool> m_bReduceGaugeOnUse;
	CEncrypt<float> m_fReduceGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessMoveState( float fTimePerSec );
	void ProcessStartState( float fTimePerSec );

	void SetDelayState();
	void SetMoveState( D3DXVECTOR3 vTargetPos );

	void FindTarget();

	void SetAimInit();

	void CheckAimTime();
	void CreateAimMark( ioPlayStage *pStage );
	void UpdateAimMark();
	void DestroyAimMark();

	bool CheckOwnerBuff();

	void CreateWeapon();

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );

public:
	ioMoveAttackDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioMoveAttackDummy();
};

inline ioMoveAttackDummy* ToioMoveAttackDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_MOVE_ATTACK )
		return NULL;

	return dynamic_cast< ioMoveAttackDummy* >( pDummy );
}