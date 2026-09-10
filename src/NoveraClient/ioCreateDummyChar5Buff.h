#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCreateDummyChar5Buff : public ioBuff
{
protected:
	bool m_bDisableDownState;

	DWORD m_dwSlerpTime;
	float m_fAlphaRate;
	bool m_bNoWoundState;
	bool m_bHideMiniMap;
	bool m_bCreatedDummy;
	bool m_bCheckCamera;

	CEncrypt<bool> m_bHideFootDustEffect;

	vDummyCharLoadInfoList m_DummyCharList;
	vDummyCharCreateInfoList m_CreateDummyList;
	ioHashStringVec m_vDestroyDummyCharList;

	CEncrypt<bool> m_bEnableWeaponSkill;
	CEncrypt<bool> m_bEnableArmorSkill;
	CEncrypt<bool> m_bEnableHelmetSkill;
	CEncrypt<bool> m_bEnableCloakSkill;

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

protected:
	void CreateDummyChar( ioBaseChar *pOwner );
	void CheckDummyChar();

public:
	D3DXVECTOR3 GetDummyCharPos();

public:
	void ApplyExtraBuffInfo( SP2Packet &rkPacket );

public:
	bool IsHideMiniMap() { return m_bHideMiniMap; }
	float GetAlphaRate() { return m_fAlphaRate; }
	bool CheckCamera() { return m_bCheckCamera; }

	bool CheckEnableUseSkill( int iSlot );
	
public:
	virtual void CheckActiveCount();

public:
	ioCreateDummyChar5Buff();
	ioCreateDummyChar5Buff( const ioCreateDummyChar5Buff &rhs );
	virtual ~ioCreateDummyChar5Buff();
};


inline ioCreateDummyChar5Buff* ToCreateDummyChar5Buff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_DUMMY_CHAR5 )
		return NULL;

	return dynamic_cast< ioCreateDummyChar5Buff* >( pBuff );
}


