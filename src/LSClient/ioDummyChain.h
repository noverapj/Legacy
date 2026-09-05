#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioDummyChain : public ioDummyChar
{			
protected:
	enum ChainActionState
	{
		CAS_Start,
		CAS_Delay,
		CAS_Move,
		CAS_Change_Rise_Key,
		CAS_Change_Dir_Key,
		CAS_Action,
		CAS_Create_Weapon,
		CAS_Teleport,
		CAS_Check_Charge,
		CAS_End,

		CAS_Circle_Move,
	};

	struct DistInfo
	{
		ioHashString szCharName;
		float fDistSq;

		bool operator<( const DistInfo &rhs ) const
		{
			return fDistSq < rhs.fDistSq;
		}
	};
	typedef std::vector< DistInfo > SortList;

protected:
	ioHashString m_szDelayAni;
	float m_fDelayAniRate;
	ioHashString m_szMoveAni;
	float m_fMoveAniRate;
	ioHashString m_szActionAni;
	float m_fActionAniRate;

	ioHashString m_szAttachBuff;

	DWORD m_dwRotateSpeed;
	DWORD m_dwLiveTime;
	DWORD m_dwChargeTime;

	int m_nTargetMaxCnt;
	float m_fTargetRange;
	WeaponInfo m_ChainWeaponInfo;
	WeaponInfo m_AttachWeaponInfo;
	
protected:
	ChainActionState m_ChainActionState;

	bool m_bTargetRot;
	bool m_bLeftRot;

	DWORD m_dwMotionEndTime;
	DWORD m_dwReserveKeyTime;

	DWORD m_dwCheckLiveTime;
	DWORD m_dwCheckChargeTime;

	SortList m_SortList;

	//Hacker
protected:
	bool m_bSetCircleMoveState;
	bool m_bDontCheckRangeDie;
	ioHashString m_szCurAttachEffect;
	bool m_bIgnoreKeyInput;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual bool IsActivateWeapon();
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual void TranslateByConveyer( const D3DXVECTOR3 &vMove );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
	virtual bool IsNeedCheckMapCollision() const;
	virtual int DontMoveEntityLevel() const;
protected:
	void SetState( ioHashString szAniName, float fAniRate, ChainActionState eState );
	void SetMoveState( ioBaseChar *pOwner );
	void SetCheckChargeState( ioBaseChar *pOwner );
	void SetActionState( ioBaseChar *pOwner );
	void SetTeleportState( ioBaseChar *pOwner );
	void SetEndState( ioBaseChar *pOwner );
	void SetCircleMoveState( ioBaseChar *pOwner );

protected:
	void ProcessMoveState( ioBaseChar *pOwner, float fTimePerSec );
	void ProcessCheckChargeState( ioBaseChar *pOwner );
	void ProcessActionState( ioBaseChar *pOwner );
	void ProcessCircleMoveState( ioBaseChar *pOwner, float fTimePerSec );

protected:
	bool CheckAttackKeyInput( ioBaseChar *pOwner );
	bool CheckDirKeyInput( ioBaseChar *pOwner );
	void CheckRiseKeyInput( ioBaseChar *pOwner );
	void CheckCircleDirKeyInput( ioBaseChar *pOwner );

protected:
	void CheckMove( ioBaseChar *pOwner, float fTimePerSec );
	void CheckDir( ioBaseChar *pOwner );
	void CheckRise( ioBaseChar *pOwner, float fTimePerSec );

protected:
	D3DXVECTOR3 GetMoveDir();

protected:
	void CreateWeapon( ioBaseChar *pOwner );
	bool FindTarget( ioBaseChar *pOwner );
	void SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList );
	bool IsEnableTarget( ioBaseChar *pTarget );
public:
	void SetIgnoreKeyInput( bool bSet ) { m_bIgnoreKeyInput = bSet; }
	void SetAttachEffect( const ioHashString& szEffectName );
public:
	ioDummyChain( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDummyChain();
};

inline ioDummyChain* ToDummyChain( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_CHAIN )
		return NULL;

	return dynamic_cast< ioDummyChain* >( pDummy );
}