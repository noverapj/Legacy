#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioDummyHelicopter : public ioDummyChar
{			
protected:
	enum DummyHelicopterState
	{
		DHS_Start,
		DHS_Delay,
		DHS_Move,
		DHS_Rise,
		DHS_Change_Key,
		DHS_Create_Weapon,
		DHS_Create_Area_Weapon,
	};

protected:
	DummyHelicopterState m_DummyHelicopterState;
	bool m_bSetRise;
	DWORD m_dwAttackTime;

protected:
	ioHashString m_szCheckBuff;
	ioHashString m_szDelayAniName;
	ioHashString m_szMoveAniName;
	ioHashString m_szRiseAniName;

	DWORD m_dwCheckOwnerDelayTime;
	CharState m_CheckOwnerState;

	ioHashString m_szAreaWeaponName;
	D3DXVECTOR3 m_vAreaWeaponOffset;
	ioHashString m_szAreaTrainingWeaponName;
	ioHashString m_szCurAreaWeaponName;

	int m_nWeaponType;
	D3DXVECTOR3 m_vWeaponOffset;
	DWORD m_dwCheckAttackTime;

	float m_fRotateWorldAngle;
	bool  m_bDontCheckDirKey;

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
	void SetRiseState( bool bRise, ioBaseChar *pOwner );
	void SetState( ioHashString szAniName, DummyHelicopterState eState );
	void ProcessDelayState();
	void ProcessMoveState();
	void ProcessRiseState();
	void ProcessEnd();

protected:
	bool CheckOwnerState();
	void CreateWeapon();
	void SetCurAreaWeaponName();
	void CreateAreaWeapon();
	void SetAreaWeaponPos();

protected:
	void CheckMove( float fTimePerSec );
	void CheckRise( float fTimePerSec );
	void CheckInputKey();

public:
	ioDummyHelicopter( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDummyHelicopter();
};

inline ioDummyHelicopter* ToDummyHelicopter( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_HELICOPTER )
		return NULL;

	return dynamic_cast< ioDummyHelicopter* >( pDummy );
}