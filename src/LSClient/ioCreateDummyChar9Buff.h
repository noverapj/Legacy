#pragma once
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioCreateDummyChar9Buff :	public ioBuff
{
protected:
	enum BuffState
	{
		BS_NONE,
		BS_CREAT_DUMMY,
		BS_RANGE_CHECK,
		BS_END,
	};
protected:
	BuffState		m_BuffState;

	ioHashString	m_szDummyName;
	D3DXVECTOR3		m_vDummyOffset;
	ioHashStringVec m_OwnerBuffList;
	DWORD			m_dwRangeCheckDisableTime;
	float			m_fEnableReturnPositionRange;

	CEncrypt<bool>	m_bHideFootDustEffect;
	CEncrypt<bool>	m_bEnableWeaponSkill;
	CEncrypt<bool>	m_bEnableArmorSkill;
	CEncrypt<bool>	m_bEnableHelmetSkill;
	CEncrypt<bool>	m_bEnableCloakSkill;

	int				m_iDummyIndex;
	DWORD			m_dwRangeCheckStartTime;

	//For Chain
	DWORD			m_dwChainWeaponIndex;
	int				m_iChainWeapon;
	D3DXVECTOR3		m_vDummyPos;

	//For Hide
	bool			m_bSlerpEnd;
	float			m_fAlphaRate;
	DWORD			m_dwSlerpTime;

	//For Effect
	ioHashString	m_szBuffMapEffectName;
	DWORD			m_dwBuffMapEffectID;
	float			m_fOwnerAttachEffectPositionRate;
	bool			m_bVisibleMapEffectOnlyOwner;
	bool m_bReturnPosition;

protected:
	void ClearData();
	
	void CreateMapEffect( ioBaseChar *pOwner );
	void CreateDummy( ioBaseChar *pOwner );
	void CreateChain( ioBaseChar *pOwner );

	void AddOwnerBuff( ioBaseChar *pOwner );
	
	bool CheckLiveBuff( ioBaseChar *pOwner );
	void CheckRange( ioBaseChar *pOwner );

	void SetChainPos( ioBaseChar *pOwner );
	void SetMapEffectPos( ioBaseChar *pOwner );
	
public:
	bool CheckEnableUseSkill( int iSlot );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();
public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

public:
	ioCreateDummyChar9Buff();
	ioCreateDummyChar9Buff( const ioCreateDummyChar9Buff &rhs );
	virtual ~ioCreateDummyChar9Buff();
};

inline ioCreateDummyChar9Buff* ToCreateDummyChar9Buff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_DUMMY_CHAR9 )
		return NULL;

	return dynamic_cast< ioCreateDummyChar9Buff* >( pBuff );
}
