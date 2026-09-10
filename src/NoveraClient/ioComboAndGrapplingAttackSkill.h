#pragma once
#include "ioattackskill.h"
class ioComboAndGrapplingAttackSkill :
	public ioAttackSkill
{
private:
	enum SkillState
	{
		SS_NONE,
		SS_TELEPORT_ATTACK,
		SS_LINEAR_COMBO,
		SS_GRAPPLING_LOOP,
		SS_LAND_ATTACK,
		SS_END,
	};

	enum SkillSyncType
	{
		SST_TELEPORT_ATTACK,
		SST_LINEAR_COMBO,
		SST_GRAPPLING_LOOP,
		SST_LAND_ATTACK,
		SST_END,
	};
private:
	SkillState	m_SkillState;

	//Teleport
	float m_fTargetRange;
	float m_fTargetAngle;
	float m_fTargetUpHeight;
	D3DXVECTOR3 m_vNoneTargetOffset;
	D3DXVECTOR3 m_vTeloportTargetOffset;
	ioHashString m_szAttackTarget;

	//Linear
	ioHashString	m_szCheckBuff;
	AttributeList m_ComboList;
	int m_iCurLinearCombo;

	//Grappling
	ioHashString m_GrapplingTargetName;
	float m_fCharJumpPower;
	float m_fCharHeightRate;
	float m_fTargetGapRange;
	ioHashString m_GrapplingLoopMotion;
	float m_fGrapplingLoopRate;
	DWORD m_dwGrapplingLoopRotateTime;
	
	
	//Land Attack
	AttackAttribute m_LandAttackAttribute;

	//Dummy
	ioHashString	m_szDummyCharName;
	D3DXVECTOR3		m_vDummyStartOffset;
	D3DXVECTOR3		m_vDummyJumppingOffset;
	float			m_fDummyHRotAngle;
	IntVec			m_vDummyComboAttackCodeList;
	int				m_iDummyGrapplingAttackCode;
	int				m_iDummyLandingAttackCode;
	DWORD			m_iDummyIndex;

private:
	void ClearData();

	void SetActionAni( ioBaseChar *pOwner, const AttackAttribute& rkAttri );
	void SetTeleportAttackState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetLinearComboState( ioBaseChar *pOwner );
	void SetGrapplingLoopState( ioBaseChar *pOwner );
	void SetLandAttackState( ioBaseChar *pOwner );
	void SetEndState();


	void FindAttTarget( ioBaseChar *pOwner, ioPlayStage *pStage );
	bool IsEnableTargetState( ioBaseChar *pTarget );

	//¸Ê Ãæµ¹Ã¼Å©
	bool CheckLanding( ioBaseChar *pOwner );

	void ProcessLoopDir( ioBaseChar *pOwner );
	
	void CreateDummyChar( ioBaseChar *pOwner, ioPlayStage *pStage );
	void DummyAttackState( ioBaseChar *pOwner, int iAttackCode );
	void DestroyDummyChar( ioBaseChar *pOwner );

	void ProcessDummyState( ioBaseChar *pOwner );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
public:
	virtual bool OnSkillStart( ioBaseChar *pOwner );
	virtual void OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pOwner );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon = NULL );
	virtual bool SetSkillGrapplingState( ioBaseChar *pOwner, const ioHashString &szTargetName );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pOwner );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket );
public:
	ioComboAndGrapplingAttackSkill(void);
	ioComboAndGrapplingAttackSkill( const ioComboAndGrapplingAttackSkill &rhs );
	~ioComboAndGrapplingAttackSkill(void);
};

inline ioComboAndGrapplingAttackSkill* ToComboAndGrapplingAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_COMBO_AND_GRAPPLING_ATTACK )
		return NULL;

	return dynamic_cast< ioComboAndGrapplingAttackSkill* >(pAttack);
}