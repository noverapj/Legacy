#pragma once


class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"

class ioGrapplingSkill2 : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_LOOP,
		SS_COMBO,
		SS_COMBO2,
		SS_DWON_GRAP,
		SS_LAND_ATTACK,
		SS_END,
	};

	enum SkillSync
	{
		SSC_TELEPORT,
		SSC_LOOP,
		SSC_COMBO,
		SSC_COMBO2,
		SSC_DOWN_GRAP,
		SSC_LAND_ATTACK,
		SSC_END,
	};

protected:
	SkillState	m_SkillState;

	// teleport
	CEncrypt<bool> m_bUseTeleport;
	CEncrypt<float> m_fTeleportSkillRange;
	CEncrypt<float> m_fTeleportSkillAngle;
	CEncrypt<float> m_fTeleportSkillOffSet;

	CEncrypt<float> m_fTeleportMoveRange;
	CEncrypt<float> m_fTeleportMoveHeightRange;

	// loop
	ioHashString	m_SkillLoopMotion;
	float			m_fSkillLoopMotionRate;

	DWORD m_dwSkillLoopStartTime;
	DWORD m_dwSkillLoopDuration;
	DWORD m_dwSkillLoopEndTime;

	DWORD m_dwEnableComboTime;

	DWORD m_dwLoopRotateTime;
	CEncrypt<float> m_fTargetLoopGapRange;

	ioHashString m_GrapplingTargetName;

	float m_fCharJumpPower;
	float m_fCharHeightRate;

	// combo
	AttributeList m_vComboList;

	int m_iCurLinearCombo;
	DWORD m_dwNextComboTime;

	DWORD m_dwSkillProtectTime;
	DWORD m_dwCurSkillProtectTime;

	// combo2
	CEncrypt<bool> m_bNoGravityCombo;

	CEncrypt<bool> m_bUseCombo2State;
	CEncrypt<float> m_fCombo2Range;
	CEncrypt<float> m_fCombo2OffSet;

	CEncrypt<int> m_iCombo2PosCnt;
	
	typedef std::deque< float > ComboPosList;
	ComboPosList m_Combo2PosList;

	// end other attack
	bool m_bUseEndOtherAttack;
	bool m_bUsedEndOtherAttack;

	AttackAttribute m_EndOtherAttack;

	//Down Grap Attack
	GrapplingSkillWoundedInfo	m_DownGrapplingInfo;
	ioHashString m_DownGrapplingAnimation;
	float m_fDownGrapplingAniRate;
	float	m_fDownGrapJumpPower;;
	//Land Attack
	AttackAttribute m_LandAttack;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEnableReserve() const;
	virtual bool IsJumpState() const;
	virtual bool IsNoDropState() const;

	virtual bool CheckUseItemMesh();

	virtual GrapplingSkillWoundedInfo GetSkillWoundGrapplingInfo();

	bool IsComboState();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

	void ProcessLoopDir( ioBaseChar *pChar );
	void ProcessRotate( ioBaseChar *pOwner );

protected:
	void SetTeleportList( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetSkillLoopState( ioBaseChar *pChar );

	void CheckLinearCombo( ioBaseChar *pChar );
	void SetNextLinearCombo( ioBaseChar *pChar, bool bOther );

	void CheckLinearCombo2( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetNextLinearCombo2( ioBaseChar *pChar, ioPlayStage *pStage );
	bool CheckTargetValidate( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetSkillEndState( ioBaseChar *pChar, bool bSendNet );

	void CheckNextLinearCombo2Pos( ioBaseChar *pChar );

	void SetDownGrapAttackState( ioBaseChar *pChar );
	void SetLandAttackState( ioBaseChar *pChar );

	bool CheckLanding( ioBaseChar *pChar );

public:
	ioGrapplingSkill2();
	ioGrapplingSkill2( const ioGrapplingSkill2 &rhs );
	virtual ~ioGrapplingSkill2();
};

inline ioGrapplingSkill2* ToGrappling2( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_GRAPPLING2 )
		return NULL;

	return dynamic_cast< ioGrapplingSkill2* >( pAttack );
}

