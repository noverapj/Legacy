#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioDummySubCrane : public ioDummyChar
{			
protected:
	enum DummySubCraneActionState
	{
		DSAS_Start,
		DSAS_Delay,
		DSAS_Attack,
		DSAS_Grap,
	};

protected:
	DummySubCraneActionState m_DummySubCraneState;
	DWORD m_dwFireTime;
	DWORD m_dwAttachWeaponIdx;

protected:
	ioHashString m_szDelayAniName;

	AttackAttribute m_AttackInfo;
	AttackAttribute m_GrapInfo;

	ioHashString m_szFindCraneDummy;
	D3DXVECTOR3 m_vDelaySubCraneOffset;

	float m_fUpSpeed;
	float m_fDownSpeed;
	float m_fMaxDownHeight;
	
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
	void SetDelayState( ioBaseChar *pOwner );
	void SetGrapState( ioBaseChar *pOwner );

public:
	void SetAttackState();

protected:
	void ProcessDelayState( ioBaseChar *pOwner );
	void ProcessAttackState( ioBaseChar *pOwner, float fTimePerSec );
	void ProcessGrapState( ioBaseChar *pOwner, float fTimePerSec );
	void ProcessEnd();
	
protected:
	void CreateWeapon( ioBaseChar *pOwner, const AttackAttribute &sCurInfo, bool bAttachIdx );
	void CheckUp( float fTimePerSec );
	void CheckDown( float fTimePerSec );
	void SetCraneDelayState( ioBaseChar *pOwner );

public:
	ioDummySubCrane( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDummySubCrane();
};

inline ioDummySubCrane* ToDummySubCrane( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_SUB_CRANE )
		return NULL;

	return dynamic_cast< ioDummySubCrane* >( pDummy );
}