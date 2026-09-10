#pragma once


#include "ioNormalSkill.h"
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMove5Skill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_ACTION,
		SS_END,
	};
	
	enum SkillSyncState
	{
		SSS_ACTION,
		SSS_UPDATE,
		SSS_DUMMY,
	};

	enum GaugeUseType
	{
		GUT_ON_KEY_INPUT,
		GUT_ON_SKILL_USE,
		GUT_BOTH,
	};

	enum CreateWeaponType
	{
		CWT_NONE,
		CWT_AFTER,
	};

protected:
	CEncrypt<int> m_State;

	DWORD m_dwFireStartTime;
	DWORD m_dwActionStartTime;

	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;
	ioHashString m_TargetCircleEffect;

	ioHashString m_RedGuidCircle;
	ioHashString m_BlueGuidCircle;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	UniqueObjID m_dwTargetCircleEffect;

	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	CEncrypt<DWORD> m_dwSkillProtectTime;
	DWORD           m_dwProtectTime;

	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

protected:
	// Attribute
	bool m_bUseAttack;

	CEncrypt<float> m_fAttackOffset;
	CEncrypt<float> m_fAttackHeightOffset;

	D3DXVECTOR3 m_vAttackDir;
	DWORD m_dwAttackCheckTime;
	CallAttributeList m_CallAttributeList;

protected:
	CEncrypt<DWORD> m_dwCircleCheckDuration;
	CEncrypt<DWORD> m_dwJumpCircleCheckDuration;
	DWORD m_dwCircleCheckTime;

	GaugeUseType m_GaugeUseType;

	CEncrypt<bool> m_bHandMeshOnDelayAni;

	//
	bool m_bChangeOwnerPos;
	bool m_bJumpCallingAttribute;

	CallAttributeList m_CallJumpAttributeList;

	// dummychar
protected:
	vDummyCharLoadInfoList m_DummyCharList;
	ioHashStringVec m_vDestroyDummyCharList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void LoadCallAttributeList( ioINILoader &rkLoader );
	void CheckCallingAttribute( ioBaseChar *pChar );

	void LoadCallJumpAttributeList( ioINILoader &rkLoader );
	void CheckCallingJumpAttribute( ioBaseChar *pChar );

	void SetWaitState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	void CheckChangePostion( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );
public:
	ioCircleMove5Skill();
	ioCircleMove5Skill( const ioCircleMove5Skill &rhs );
	virtual ~ioCircleMove5Skill();
};

inline ioCircleMove5Skill* ToCircleMove5Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE5 )
		return NULL;

	return dynamic_cast< ioCircleMove5Skill* >( pSkill );
}