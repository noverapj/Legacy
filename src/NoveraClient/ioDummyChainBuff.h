#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioDummyChainBuff : public ioBuff
{
public:
	enum BuffState
	{
		BS_CreateDummy,
		BS_START,
		BS_WAIT,
		BS_MOVE,
		BS_END,
	};
	
protected:
	ioHashString m_szDummyName;
	D3DXVECTOR3 m_vDummyOffset;
	int m_nChainWeapon;

	ioHashString m_szMoveEffect;
	D3DXVECTOR3 m_vMoveEffectOffset;
	float m_fMoveSpeed;
	float m_fArriveDiff;

	bool m_bEnableWeaponSkill;
	bool m_bEnableArmorSkill;
	bool m_bEnableHelmetSkill;
	bool m_bEnableCloakSkill;

	bool m_bSetEndJump;
	float m_fEndJumpPower;
	float m_fFloatDiff;

	ioHashString m_szArriveEffect;

protected:
	BuffState m_BuffState;
	DWORD m_dwChainWeaponIndex;
	bool m_bSetInvisible;
	DWORD m_dwMoveEffectID;
	DWORD m_dwArriveEffectID;
	D3DXVECTOR3 m_vArrivePos;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );
	virtual bool IsCollisionAvailableWeapon() const;

protected:
	void CreateDummy( ioBaseChar *pOwner );
	void CreateChain( ioBaseChar *pOwner );
	void SetChainPos( ioBaseChar *pOwner );

protected:
	void CheckWaitState( ioBaseChar *pOwner );

protected:
	void SetMoveState( ioBaseChar *pOwner );
	void SetEndState( ioBaseChar *pOwner );

protected:
	void ProcessMove( ioBaseChar *pOwner );

public:
	bool IsCharCollisionSkipState();
	bool CheckEnableUseSkill( int nSlot );
	const bool IsHideState() { return m_bSetInvisible; }

protected:
	DWORD CreateMapEffect( ioBaseChar *pOwner, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale );
	void DestroyMapEffect( ioBaseChar *pOwner, DWORD& dwEffectID );
	void UpdateMapEffect( ioBaseChar *pOwner );

	bool CheckpointX( int nX, int nY );

public:
	ioDummyChainBuff();
	ioDummyChainBuff( const ioDummyChainBuff &rhs );
	virtual ~ioDummyChainBuff();
};

inline ioDummyChainBuff* ToDummyChainBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DUMMY_CHAIN )
		return NULL;

	return dynamic_cast< ioDummyChainBuff* >( pBuff );
}