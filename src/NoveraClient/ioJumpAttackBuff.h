#pragma once
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioJumpAttackBuff :public ioBuff
{
protected:
	enum BuffState
	{
		NONE,
		BS_ATTACK,
	};
protected:
	BuffState			m_BuffState;
	float				m_fNeedGauge;
	DWORD				m_dwMotionEndTime;
	AttackAttribute		m_ExtraAttack;
	float				m_fEndJumpPower;
	DWORD				m_dwFireStartTime;

	//Target
	float				m_fAimAngle;
	float				m_fMinRange;
	float				m_fMaxRange;
	bool				m_bEnableDownAim;
protected:
	void		SetActionState( ioBaseChar* pChar, ioPlayStage *pStage );
	void		SetActionAni( ioBaseChar* pChar, const AttackAttribute& Attri );
	void		SetActionEndState( ioBaseChar *pChar );
	void		CreateWeapon( ioBaseChar *pChar );
public:
	bool		CheckEnableJumpAttackState( ioBaseChar *pOwner );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();
public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );
public:
	ioJumpAttackBuff(void);
	ioJumpAttackBuff( const ioJumpAttackBuff &rhs );
	virtual ~ioJumpAttackBuff(void);
};

inline ioJumpAttackBuff* ToJumpAttackBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_BUFF_JUMP_ATTACK )
		return NULL;

	return dynamic_cast< ioJumpAttackBuff* >( pBuff );
}