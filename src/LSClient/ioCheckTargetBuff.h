#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCheckTargetBuff : public ioBuff
{
protected:
	struct DistInfo
	{
		ioHashString szCharName;
		float fDistSq;

		bool operator<( const DistInfo &rhs ) const
		{
			return fDistSq < rhs.fDistSq;
		}
	};
	typedef std::vector< DistInfo > SortList;

	enum BuffState
	{
		BS_Find,
		BS_Attack,
	};

protected:
	float m_fTargetColRange;
	float m_fTargetColAngle;
	float m_fTargetColUpHeight;
	float m_fTargetColUnderHeight;
	ioHashString m_TargetBuff;
	float m_fGaugeDec;
	int m_nEquipSlot;
	bool m_bEnableTargetDown;

protected:
	SortList m_SortList;
	BuffState m_BuffState;
	ioHashString m_szCurTarget;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );
	
protected:
	bool FindTarget( ioBaseChar *pOwner, float fColRange, float fColAngle, float fUpHeight, float fUnderHeight, bool bChangeTarget, bool bCheckBuff );
	void SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList, bool bCheckBuff );
	bool IsEnableTarget( ioBaseChar *pOwner, ioBaseChar *pTarget, bool bCheckBuff );
	bool IsCompareTarget( ioBaseChar *pOwner );

protected:
	void AddTargetBuff( ioBaseChar *pOwner, ioBaseChar *pTarget );
	void RemoveTargetBuff( ioBaseChar *pOwner, ioBaseChar *pTarget );

protected:
	void SetFindState( ioBaseChar *pOwner );
	void SetAttackState( ioBaseChar *pOwner );

protected:
	void UpdatePassiveGauge( ioBaseChar *pOwner );
	bool IsProcess( ioBaseChar *pOwner );

public:
	ioCheckTargetBuff();
	ioCheckTargetBuff( const ioCheckTargetBuff &rhs );
	virtual ~ioCheckTargetBuff();
};

inline ioCheckTargetBuff* ToCheckTargetBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CHECK_TARGET )
		return NULL;

	return dynamic_cast< ioCheckTargetBuff* >( pBuff );
}