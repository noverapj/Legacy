#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCreateWeaponCountBuff : public ioBuff
{
public:
	enum BuffState
	{
		BS_DELAY,
		BS_FIRE,
	};

	//추가//////////////////////////
	enum CreateWeaponByType
	{
		CWBT_NORMAL,
		CWBT_COUNT
	};
	///////////////////////////////

protected:
	BuffState m_BuffState;

	bool m_bDisableDownState;

	CEncrypt<DWORD> m_dwFireGapTime;
	DWORD m_dwCheckFireTime;

	ioHashString m_szDelayEffect;
	DWORD m_dwDelayEffectID;
	ioHashString m_szFireEffect;

	DWORD m_dwCheckGavityTime;
	CEncrypt<DWORD> m_dwGravityTime; 
	CEncrypt<float> m_fGravityRate;

	WeaponInfo m_WeaponInfo;

	ioHashString m_szFireSound;

	//추가 변수////////////////////////////////////////////////////////
protected:
	//무기 생성 타입은?
	CreateWeaponByType m_CreateWeaponType;
	//총 무기 생성 횟수
	int m_nTotalWeaponCnt;
	//현재 무기 사용 횟수
	int m_nCurWeaponCnt;
	///////////////////////////////////////////////////////////////////

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();
	void ProcessKeyInput();
	void ProcessState();
	void CreateWeapon();

public:
	void ApplyExtraBuffInfo( SP2Packet &rkPacket );

	//추가함수////////////////////////////////////
public:
	//추가 읽기
	void LoadWeaponCreateProperty( ioINILoader &rkLoader );
	//스킬게이지 설정
	void SetSkillGauge( float fSkillGauge );
	/////////////////////////////////////////////////////////////

public:
	ioCreateWeaponCountBuff();
	ioCreateWeaponCountBuff( const ioCreateWeaponCountBuff &rhs );
	virtual ~ioCreateWeaponCountBuff();
};


inline ioCreateWeaponCountBuff* ToNewCreateWeaponBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_WEAPON_COUNT )
		return NULL;

	return dynamic_cast< ioCreateWeaponCountBuff* >( pBuff );
}