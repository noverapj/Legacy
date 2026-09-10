#pragma once

#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioDummyStruct : public ioDummyChar
{			
protected:
	enum DummyStructState
	{
		DSS_START,
		DSS_LOOP,
	};
	enum
	{
		SYNC_GARDENER_TARGET = 1000,
	};

protected:
	ioHashString m_szCheckBuff;
	DummyStructState m_DummyStructState;
	bool m_bSetAllWeaponDefence;
	ioHashStringVec m_OwnerBuffList;
	ioHashStringVec m_RemoveBuffList;
	bool m_bCheckRange;
	float m_fRange;
	bool m_bOnlyOwnerAddBuff;

	bool m_bPowerChargeShieldDummy;

	ioHashStringVec m_AreaWeaponTrigerNameVec;
	WeaponInfoList m_TrigerAttachWeaponInfoList;
	DWORDVec m_vTrigerAttachWeaponIndexList;
	bool m_bTrigerWeapon;

	ioHashStringVec m_HealAreaWeaponNameVec;
	DWORD m_dwHealStartTime;
	DWORD m_dwHealTime;
	DWORD m_dwHealTicTime;
	float m_fHealHp;

	ioHashStringVec m_TrigerAreaWeaponVec;
	DWORDVec m_vTrigerAreaWeaponIndexList;

	bool m_bCheckGardenerSpecial;
	float m_fGardenerSpecialRange;
	float m_fGardenerSpecialEndJumpPower;
	D3DXVECTOR3 m_vGardenerSpecialOffset;
	ioHashString m_szGardenerSpecialTargetName;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
	virtual bool IsActivateWeapon();
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual void TranslateByConveyer( const D3DXVECTOR3 &vMove );
	virtual bool IsAllWeaponDefence() const;
	virtual int DontMoveEntityLevel() const;
	virtual bool IsDummyVsDummyCollisionSkipState( const ioDummyChar *pDummy );
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

	void CheckHealHp();

	D3DXVECTOR3 GetGardenerOffset()					{ return m_vGardenerSpecialOffset; }
	void SetGardenerTarget( ioHashString szName )	{ m_szGardenerSpecialTargetName = szName; }

protected:
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void LoadRemoveBuffList( ioINILoader &rkLoader );

protected:
	void CheckBuff();
	void ProcessEnd();
	void SetOwenrBuff();
	void CheckRange();
	void CheckGardenerSpecial();
	void SetGardenerSpecial( ioBaseChar* pChar );

	void CreateTrigerAttachWeapon();
	void DestoryTrigerAttachWeapon();
	void CreateTrigerAreaWeapon();
	void DestoryTrigerAreaWeapon();

public:
	ioDummyStruct( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDummyStruct();
};

inline ioDummyStruct* ToDummyStruct( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_STRUCT )
		return NULL;

	return dynamic_cast< ioDummyStruct* >( pDummy );
}