#pragma once

#include "ioBuff.h"

class ioINILoader;
class ioBaseChar;

class ioSkipStunBuff : public ioBuff
{
public:
	enum WeaponAttachType
	{
		WAT_NONE,
		WAT_XZ,
		WAT_ALL,
		WAT_AREA_XZ,
		WAT_ALL_BY_WEAPON,
		WAT_ALL_BY_WEAPON2,
	};

protected:
	ioHashString		m_StunAnimation;
	ioHashString		m_CatchAnimation;

	WeaponAttachType	m_WeaponAttachType;
	ioHashStringVec		m_WeaponDeadBuffList;
	ioHashStringVec		m_WeaponDeadByMapCollBuffList;

	CEncrypt<bool>		m_bLiveOnWeapon;
	DWORD				m_dwCheckDeadWeaponAttrIndex;
	ioHashString		m_SkillName;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );

protected:
	bool CheckWeaponAttach();
	void CheckWeaponDieCheck();

	void SetWeaponDeadCancel();
	void SetWeaponDeadCancelByMap();

public:
	ioSkipStunBuff();
	ioSkipStunBuff( const ioSkipStunBuff &rhs );
	virtual ~ioSkipStunBuff();
};

inline ioSkipStunBuff* ToSkipStunBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_SKIP_STUN )
		return NULL;

	return dynamic_cast< ioSkipStunBuff* >( pBuff );
}