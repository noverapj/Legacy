#pragma once

#include "ioCountSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioCountRollingSkill : public ioCountSkill
{
protected:
	enum SkillState
	{
		SS_PRE,
		SS_ROLLING,
		SS_JUMP_ROLLING,
		SS_END,
	};

	typedef ioUserKeyInput::DirKeyInput DirKey;

protected:
	SkillState		m_SkillState;
	DirKey			m_iCurDirKey;
	BuffInfoList	m_OwnerBuffList;

protected:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsCanUseSkill( ioBaseChar *pOwner, bool bHelpMsg );

public:
	virtual bool OnSkillStart( ioBaseChar *pOwner );
	virtual void OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pOwner );

protected:
	void SetOwnerBuff( ioBaseChar *pOwner );

	void SetRollingState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetJumpRollingState( ioBaseChar *pOwner, ioPlayStage *pStage );

	void CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckRollingState( ioBaseChar *pOwner, ioPlayStage *pStage );

public:
	virtual void ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsCanSendControl() const;

public:
	ioCountRollingSkill();
	ioCountRollingSkill( const ioCountRollingSkill &rhs );
	virtual ~ioCountRollingSkill();
};

inline ioCountRollingSkill* ToCountRollingSkill( ioSkill *pSkill )
{
	ioCountSkill *pRange = ToCountSkill( pSkill );
	if( !pRange || pRange->GetSubSkillType() != CST_ROLLING )
		return NULL;

	return dynamic_cast< ioCountRollingSkill* >(pRange);
}