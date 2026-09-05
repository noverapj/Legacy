#pragma once

#include "ioNormalSkill.h"

class ioINILoader;
class ioBaseChar;
class ioItem;

class ioJumpLinearComboSkill : public ioNormalSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_JUMP,
		SS_LANDING,
		SS_LINECOMBO,
		SS_END,
	};

	enum SkillSync
	{
		SSC_JUMP,
		SSC_LANDING,
		SSC_LINECOMBO,
		SSC_END,
	};

	AttackAttribute m_JumpMotionAttribute;
	AttackAttribute m_LandAttackAttribute;
	AttributeList  m_vLinearAttackAttribute;
	CEncrypt<float> m_fJumpPower;

	SkillState     m_SkillState;

	D3DXVECTOR3 m_vPrePosition;
	D3DXVECTOR3 m_vMoveDirection;
	float m_fLastHeight;

	int m_nCurLinearComboIndex;

	DWORD m_dwAttackEndTime;

	bool m_bReserveLinearCombo;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	DWORD m_dwSkillProtectTime;


protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	void ProcessJumpState( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessLandingState( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessLinearAttackState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetJumpAction( ioBaseChar *pChar, bool bSend );
	void SetLanding( ioBaseChar *pChar, bool bSend );
	void SetEndState( ioBaseChar *pChar, bool bSetEndMotion );
	void SetLinearComboState( ioBaseChar *pChar, int nComboIndex );

	bool CheckLanding( ioBaseChar *pChar );
	//bool CheckEnableKeyInput( ioBaseChar *pChar, ioPlayStage *pStage );
	//bool CheckReserveKeyInput( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckRotateByKeyInput( ioBaseChar *pChar );

protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );

public:
	ioJumpLinearComboSkill();
	ioJumpLinearComboSkill( const ioJumpLinearComboSkill &rhs );
	virtual ~ioJumpLinearComboSkill();
};

inline ioJumpLinearComboSkill* ToJumpLinearComboSkill( ioSkill *pSkill )
{
	if( !pSkill || pSkill->GetTypeID() != NST_JUMP_LINEAR_COMBO )
		return NULL;

	return dynamic_cast< ioJumpLinearComboSkill* >( pSkill );
}
