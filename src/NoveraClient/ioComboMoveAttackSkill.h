#pragma once


#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioComboMoveAttackSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_COMBO,
	};

	enum SkillSync
	{
		SSC_ACTION,
		SSC_COMBO,
	};

protected:
	CEncrypt<int> m_State;

	// combo
	AttributeList m_vComboList;
	DWORDVec m_ComboInputTimeList;
	ioHashStringVec m_ComboInputAniList;

	int m_iCurLinearCombo;
	DWORD m_dwComboInputEnableTime;
	DWORD m_dwNextComboTime;

	ioHashString m_CurInputAni;
	bool m_bSetInputAni;

	DWORD m_dwSkillProtectTime;
	DWORD m_dwCurSkillProtectTime;

	D3DXVECTOR3 m_vCharMoveDir;

	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetLinearComboState( ioBaseChar *pChar );

	void CheckLinearCombo( ioBaseChar *pChar );
	void SetNextLinearCombo( ioBaseChar *pChar, bool bSend );

	void SetPreActionState( ioBaseChar *pChar );

public:
	ioComboMoveAttackSkill();
	ioComboMoveAttackSkill( const ioComboMoveAttackSkill &rhs );
	virtual ~ioComboMoveAttackSkill();
};

inline ioComboMoveAttackSkill* ToComboMoveAttackSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_COMBO_MOVE_ATTACK )
		return NULL;

	return dynamic_cast< ioComboMoveAttackSkill* >( pSkill );
}

