#pragma once


#include "ioMachineStruct.h"

class ioMachineCannon : public ioMachineStruct
{
public:
	enum CannonState
	{
		MCS_NONE,
		MCS_DELAY,
		MCS_FIRE,
	};

protected:
	CannonState m_CannonState;

	bool m_bTargetRot;
	DWORD m_dwRotateTime;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	bool m_bAttackKey;

	DWORD m_dwChangeStateTime;
	DWORD m_dwSetTakeTime;

	WeaponInfo m_WeaponInfo;
	
	CEncrypt<DWORD> m_dwFireTime;
	CEncrypt<DWORD> m_dwFireEndTime;

	int m_iBaseWeaponIndex;
	int m_iWeaponFireCnt;

	CEncrypt<float> m_fWeaponOffSet;
	CEncrypt<float> m_fWeaponSideOffSet;
	CEncrypt<float> m_fWeaponHeightOffSet;

	ioHashString m_NoneAnimation;
	ioHashString m_DelayAnimation;
	ioHashString m_FireAnimation;
	CEncrypt<float> m_fFireAniRate;

	D3DXVECTOR3 m_ReleaseOffSet;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual MachineStructType GetType() const;

protected:
	virtual void ProcessState( float fTimerPerSec );

public:
	virtual void InitMachineStructInfo( int iStructNum, int iArrayNum, DWORD dwCreateGapTime, DWORD dwSeed, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale );

	virtual void SetStructDie( ioBaseChar *pChar, bool bEffect );
	virtual void StructDie( bool bEffect=true );

	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );
	virtual bool IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const;

	virtual bool IsTakeEnableState();

	virtual void SetTakeState( const ioHashString &szTakeCharName );
	virtual void ReleaseState();

	virtual void ApplyMachineStructExtraInfo( SP2Packet &rkPacket );

protected:
	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );
	
	void ProcessNone( float fTimePerSec );
	void ProcessDelay( float fTimePerSec );
	void ProcessFire( float fTimePerSec );

	void ChangeToFireState();

	void ApplyMachineState( SP2Packet &rkPacket );

public:
	ioMachineCannon( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioMachineCannon();
};

