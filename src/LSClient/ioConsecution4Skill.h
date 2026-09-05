#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioConsecution4Skill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};

	enum HandState
	{
		HS_LEFT,
		HS_RIGHT,
	};
	
	enum
	{
		SST_ACTION,
	};

protected:
	SkillState m_State;
	HandState m_HandState;

	// 스킬 게이지 감소
	CEncrypt<bool> m_bDisableGrowth;
	CEncrypt<float> m_fGuagePerTic;
	CEncrypt<DWORD> m_dwTicTime;

	DWORD m_dwLifeTime;

	DWORD m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	ioHashString m_szPreLoopMotion;
	ioHashString m_szRunLoopMotion;
	bool m_bSetMoveAni;

	ioHashString m_szLeftHandAction;
	ioHashString m_szRightHandAction;
	float m_fLeftHandActionRate;
	float m_fRightHandActionRate;

	ioHashString m_szRunLeftHandAction;
	ioHashString m_szRunRightHandAction;
	float m_fRunLeftHandActionRate;
	float m_fRunRightHandActionRate;
	
	DWORD m_dwFireMotionStartTime;

	WeaponInfo m_LeftWeapon;
	WeaponInfo m_RightWeapon;

	bool m_bFire;

	TargetWoundType m_TargetWoundType;
	CEncrypt<float>	m_fLeftTargetMinAngle;
	CEncrypt<float>	m_fLeftTargetMaxAngle;
	CEncrypt<float> m_fRightTargetMinAngle;
	CEncrypt<float>	m_fRightTargetMaxAngle;

	CEncrypt<float>	m_fTargetRange;
	CEncrypt<float>	m_fTargetMinRange;

	D3DXVECTOR3 m_vAttackDir;

	UniqueObjID m_dwLeftAimEffectID;
	UniqueObjID m_dwRightAimEffectID;
	ioHashString m_szAimMarker;
	
	ioHashString m_szLeftTargetName;
	ioHashString m_szRightTargetName;
	BaseCharDistanceSqList m_szLeftTargetNameList;
	BaseCharDistanceSqList m_szRightTargetNameList;

	CEncrypt<DWORD> m_dwSkillProtectedTime;
	DWORD m_dwStartSkillProtectedTime;

	DWORD m_dwFireMotionLoopCnt;
	DWORD m_dwCurFireMotionLoopCnt;

	BuffInfoList m_OwnerBuffList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void LoadAttackAttribute( ioINILoader &rkLoader );
	void LoadOwnerBuffList( ioINILoader &rkLoader );

protected:
	void ClearData();

	void SetActionState( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetLoopState( ioBaseChar* pChar, ioPlayStage* pStage );

	void CheckNormalMoveAni( ioBaseChar* pChar );

	void FindTarget( ioBaseChar* pChar, ioPlayStage* pStage );
	void FindLeftTarget( ioBaseChar* pChar, ioPlayStage* pStage );
	void FindRightTarget( ioBaseChar* pChar, ioPlayStage* pStage );

	bool IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound );

	void CreateMarker( ioBaseChar *pChar, ioPlayStage *pStage );
	void UpdateMarker( ioPlayStage *pStage );
	void DestroyMarker( ioBaseChar *pChar );

	void SetOwnerBuff( ioBaseChar *pChar );
	void RemoveOwnerBuff( ioBaseChar *pChar );

public:
	ioConsecution4Skill(void);
	ioConsecution4Skill( const ioConsecution4Skill &rhs );
	virtual ~ioConsecution4Skill();
};

inline ioConsecution4Skill* ToConsecution4Skill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CONSECUTION4 )
		return NULL;

	return dynamic_cast< ioConsecution4Skill* >( pAttack );
}