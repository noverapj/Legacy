//////////////////////////////////////////////////////////////////////////
//작성 : 윤태철
//설명 : 악령 도둑 스킬중에 스킬을 사용하여 weapon을 발사하여 타겟을 가격시 
//       스킬게이지중 꽉찬거가 존재한다면 삭제를 하고 다시 공격자에게 발사체를 생성하여
//	     공격자에 닿으면 게이지를 회복시켜준다.         
//	     만약 꽉찬 스킬게이지가 존재하지 않으면 공격자에게 다른 발사체를 생성한다.
//       ( 발사체는 buff create기능이 있다. 각 무기별로 셋팅이 된다. )
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioSkillGaugeCheckBuff : public ioBuff
{
	int m_nSuccessWeapon;
	int m_nFailWeapon;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual void ApplyExtraBuffInfo(  SP2Packet &rkPacket  );

protected:
	void CreateSuccusWeapon( bool bSend );
	void CreateFailWeapon( bool bSend );

public:
	ioSkillGaugeCheckBuff();
	ioSkillGaugeCheckBuff( const ioSkillGaugeCheckBuff &rhs );
	virtual ~ioSkillGaugeCheckBuff();
};


inline ioSkillGaugeCheckBuff* ToSkillGaugeCheckBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_SKILL_GAUGE_CHECK )
		return NULL;

	return dynamic_cast< ioSkillGaugeCheckBuff* >( pBuff );
}


