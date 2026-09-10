#pragma once

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioComboMoveAttack2Skill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_ACTION,
		SS_WAIT,
		SS_ACTION_LOOP,
		SS_ACTION,
		SS_COMBO,
		SS_END,
	};

	enum SkillSync
	{
		SSC_ACTION,
		SSC_COMBO,
	};

protected:
	CEncrypt<int> m_State;

	// preaction
	DWORD m_dwPreActionEndTime;

	// wait
	ioHashString m_WaitAni;
	DWORD m_dwWaitStartTime;

	CEncrypt<DWORD> m_dwWaitProtectionTime;
	CEncrypt<DWORD> m_dwWaitDuration;

	// Circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	ioHashString m_ReadyCircle;
	ioHashString m_TargetCircleEffect;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	UniqueObjID m_dwTargetCircleEffect;

	D3DXVECTOR3 m_vCirclePos;

	// action loop
	ioHashString m_ActionLoopAni;

	float m_fCharJumpPower;
	float m_fCurCharMoveSpeed;
	D3DXVECTOR3 m_vCharMoveDir;

	// Action
	AttackAttribute m_LandAttackAttribute;

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

	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	DWORD			m_dwCheckStartTime;
	CEncrypt<DWORD>	m_dwStartTime;

	CEncrypt<float>	m_fBackRange;
	CEncrypt<float>	m_fHeightRange;

	DWORD m_dwAttackEndTime;

	ioHashStringVec	m_TargetNameList;

protected:
	void ClearData();
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
	virtual bool IsAttackEndState() const;

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsNoDropState() const;

protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );
	void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetLinearComboState( ioBaseChar *pChar );

	void CheckLinearCombo( ioBaseChar *pChar );
	void SetNextLinearCombo( ioBaseChar *pChar, bool bSend );

	void SetSkillMove( ioPlayStage *pStage, ioBaseChar *pChar );
	void SetSkillEndState();

	void SetPreAction( ioBaseChar *pChar );
	void SetWaitState( ioBaseChar *pChar );
	void SetActionLoopState( ioBaseChar *pChar );

	bool CheckLanding( ioBaseChar *pChar );
	void ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage );
	float GetFlightDuration( const D3DXVECTOR3 &vCharPos, float fCurGravity );

public:
	virtual void SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock );

public:
	ioComboMoveAttack2Skill();
	ioComboMoveAttack2Skill( const ioComboMoveAttack2Skill &rhs );
	virtual ~ioComboMoveAttack2Skill();
};

inline ioComboMoveAttack2Skill* ToComboMoveAttack2Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_COMBO_MOVE_ATTACK2 )
		return NULL;

	return dynamic_cast< ioComboMoveAttack2Skill* >(pNormal);
}

