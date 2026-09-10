#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioDummyCrane : public ioDummyChar
{			
protected:
	enum DummyCraneActionState
	{
		DAS_Start,
		DAS_Delay,
		DAS_Move,
		DAS_Rise,
		DAS_Change_Key,
		DAS_Sub_Action,
		DAS_Create_Sub_Dummy,
		DAS_End,
	};

protected:
	DummyCraneActionState m_DummyCraneState;
	bool m_bSetRise;
	DWORD m_dwCurReserveEndTime;
	int m_nSkillActiveCurCnt;	

protected:
	ioHashString m_szDelayAniName;
	ioHashString m_szMoveAniName;
	ioHashString m_szRiseAniName;
	ioHashString m_szActionAniName;
	
	ioHashString m_szSubDummyName;
	D3DXVECTOR3 m_vSubDummyOffset;

	float m_fMinDownHeight;

	ioHashString m_szGrapBuff;

	int m_nSkillActiveMaxCnt;

	DWORD m_dwReserveEndTime;
	
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
	virtual int GetSkillMaxActiveCnt();
	virtual int GetSkillCurActiveCnt();

protected:
	void SetState( ioHashString szAniName, DummyCraneActionState eState );
	void SetDelayState( ioBaseChar *pOwner );
	void SetMoveState( ioBaseChar *pOwner );
	void SetRiseState( bool bRise, ioBaseChar *pOwner );
	void SetActionState( ioBaseChar *pOwner );

	void ProcessDelayState();
	void ProcessMoveState();
	void ProcessRiseState();
	void ProcessEnd();
	
public:
	void ResetCraneState();

protected:
	void CheckMove( float fTimePerSec );
	void CheckRise( float fTimePerSec );
	void CheckRiseInputKey();

protected:
	void CreateSubDummy();

public:
	ioDummyCrane( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDummyCrane();
};

inline ioDummyCrane* ToDummyCrane( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_CRANE )
		return NULL;

	return dynamic_cast< ioDummyCrane* >( pDummy );
}