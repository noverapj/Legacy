#pragma once

#include "ioRangeSkill.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioRangeTeleportSkill2 : public ioRangeSkill
{
public:
	enum SyncType
	{
		ST_TELEPORT,
		ST_COMBO,
	};

protected:
	float m_fBackRange;
	float m_fHeightRange;

protected:
	AttributeList m_vComboList;

	int m_iCurLinearCombo;

	DWORD m_dwAttackEndTime;

	DWORD m_dwSkillProtectTime;
	DWORD m_dwCurSkillProtectTime;

	CEncrypt<bool> m_bNoGravityCombo;
	CEncrypt<bool> m_bUsedComboList;
	CEncrypt<bool> m_bSetAttackFire;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<bool> m_bSelectTeleportDirection;
	CEncrypt<float> m_fRangeEmptyTarget;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsAttackEndState() const;

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEnableReserve() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual bool IsJumpState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsCanSendControl() const;

protected:
	void SetTeleportList( ioBaseChar *pOwner, ioPlayStage *pStage );

	void SetSkillActionState( ioBaseChar *pChar );

	void CheckLinearCombo( ioBaseChar *pChar );
	void SetNextLinearCombo( ioBaseChar *pChar, bool bSend );

public:
	ioRangeTeleportSkill2();
	ioRangeTeleportSkill2( const ioRangeTeleportSkill2 &rhs );
	virtual ~ioRangeTeleportSkill2();
};

inline ioRangeTeleportSkill2* ToRangeTeleportSkill2( ioSkill *pSkill )
{
	ioRangeSkill *pRange = ToRangeSkill( pSkill );
	if( !pRange || pRange->GetSubSkillType() != RST_TELEPORT2 )
		return NULL;

	return dynamic_cast< ioRangeTeleportSkill2* >(pRange);
}

