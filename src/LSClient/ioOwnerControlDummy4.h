#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioOwnerControlDummy4 : public ioDummyChar
{			
protected:
	enum ControlState
	{
		CS_None,
		CS_Start,
		CS_Control_Move,
		CS_Return_Move,
		CS_End,
	};

	typedef struct tagOwnerHealInfo
	{
		int m_MinAttackCnt;
		int m_MaxAttackCnt;
		ioHashString m_szHealBuff;
	}OwnerHealInfo;

	typedef std::vector<OwnerHealInfo> OwnerHealInfoList;

protected:
	ControlState m_ControlState;
	DWORD dwCheckAttackTime;
	int m_nAttackCnt;

protected:
	ioHashString m_szControlMoveAni;
	float m_fControlMoveAniRate;

	ioHashString m_szControlCheckBuff;
	ioHashString m_szAttackCheckBuff;

	float m_fAttackRange;
	float m_fAttackAngle;
	float m_fAttackUpHeight;
	float m_fAttackUnderHeight;

	DWORD dwAttackTime;
	int m_nWeaponType;

	ioHashString m_szReturnMoveAni;
	float m_fReturnMoveAniRate;
	float m_fReturnMoveSpeed;
	float m_fReturnDist;
	
	OwnerHealInfoList m_OwnerHealInfoList;
	ioHashString m_szHealOtherBuff;
	int m_nMaxAttackCnt;

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

	virtual bool IsMountingAvailable() const;
	virtual bool IsMountAvailable() const;
	virtual bool IsNowMountEnable() const;
	
protected:
	void SetControlMoveState();
	void CheckControlKey( ioBaseChar *pOwner );
	void ProcessControlMove( ioBaseChar *pOwner, float fTimePerSec );

	void SetReturnMoveState();
	void CheckReturnMove( ioBaseChar *pOwner );
	void ProcessReturnMove( ioBaseChar *pOwner, float fTimePerSec );
	void CheckReturnEnd( ioBaseChar *pOwner );

protected:
	void FindTarget( ioBaseChar *pOwner, float fTargetRange, float fTargetAngle, float fTargetUpheight, float fTargetUnderHeight, ioHashStringVec &szTargetList );
	void CreateWeapon( ioBaseChar *pOwner );
	void HealOwnerBuff( ioBaseChar *pOwner );

public:
	ioOwnerControlDummy4( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioOwnerControlDummy4();
};

inline ioOwnerControlDummy4* ToOwnerControlDummy4( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_OWNER_CONTROL4 )
		return NULL;

	return dynamic_cast< ioOwnerControlDummy4* >( pDummy );
}