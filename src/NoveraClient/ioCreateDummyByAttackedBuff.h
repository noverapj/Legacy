#pragma once
#include "iobuff.h"
class ioCreateDummyByAttackedBuff :
	public ioBuff
{
protected:
	enum Buff_Sync
	{
		BS_CREATE_DUMMY,
		BS_END,
	};
protected:
	vDummyCharLoadInfoList m_DummyList;
	ioHashStringVec m_vDestroyDummyList;

	//피격 관련
	int				m_iMaxAttackedCount;		//더미 소환을 위해 필요한 피격 횟수
	int				m_iCurAttackedCount;		//현재 피격 횟수
	
	//더미 생성 관련
	int				m_iMaxCreateDummyCount;		//생성될 수 있는 최대 더미 수
	int				m_iCurCreatedDummyCount;	//현재까지 생성된 더미의 수
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );
	virtual void ProcessBuff( float fTimePerSec );
public:
	void OwnerCharApplyWeapon();
	void CreateDummyChar( ioBaseChar *pOwner );
public:
	ioCreateDummyByAttackedBuff(void);
	ioCreateDummyByAttackedBuff( const ioCreateDummyByAttackedBuff &rhs );
	virtual ~ioCreateDummyByAttackedBuff(void);
};

inline ioCreateDummyByAttackedBuff* ToCreateDummyByAttackedBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_DUMMY_BY_ATTACKED_BUFF )
		return NULL;

	return dynamic_cast< ioCreateDummyByAttackedBuff* >( pBuff );
}