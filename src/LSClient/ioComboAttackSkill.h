#pragma once


#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioComboAttackSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_LOOP,
		SS_END,
	};

	enum SkillSync
	{
		SSC_ACTION,
		SSC_LOOP,
	};

protected:
	CEncrypt<int> m_State;

	// float
	CEncrypt<float> m_fReduceGauge;
	// range effect
	CEncrypt<float>	m_fRangeMaxRadius;
	CEncrypt<float>	m_fRangeMinRadius;	
	CEncrypt<float>	m_fRangeSpeed;
	ioHashString	m_szRangeEffect;
	ioHashString	m_szRangeArcEffect;
	FloatVec		m_ArcEffectAngleList;
	float			m_fRangeCurRadius;
	DWORD			m_dwRangeEffect;
	DWORDVec		m_dwRangeArcEffectList;
	DWORD			m_dwActionStartTime;

	// combo
	CEncrypt<int>	m_iComboCount;
	AttributeList	m_vComboList;
	DWORDVec		m_ComboInputTimeList;
	ioHashStringVec	m_ComboInputAniList;

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
	void SetActionState( ioBaseChar *pChar );
	void SetLinearComboState( ioBaseChar *pChar );
	void SetEndState();

	void CheckLinearCombo( ioBaseChar *pChar );
	void SetNextLinearCombo( ioBaseChar *pChar, bool bSend );

	void CreateRangeEffect( ioBaseChar *pChar );
	void UpdateRangeEffect( ioPlayStage *pStage );
	void CreateRangeArcEffect( ioBaseChar *pChar );

public:
	ioComboAttackSkill();
	ioComboAttackSkill( const ioComboAttackSkill &rhs );
	virtual ~ioComboAttackSkill();
};

inline ioComboAttackSkill* ToComboAttackSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_COMBO_ATTACK )
		return NULL;

	return dynamic_cast< ioComboAttackSkill* >( pSkill );
}

