#pragma once


class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"


class ioConsecution2Skill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
	};

	enum Skillsync
	{
		SSC_START_FIRE,
		SSC_ON_FIRE,
	};

protected:
	SkillState m_SkillState;
	
	DWORD			m_dwLoopStartTime;

	ioHashString	m_szTargetMarker;
	CEncrypt<float>	m_fMarkerOffset;
	DWORD			m_dwTargetMarkerID;
	ioHashString	m_szResultMarker;
	DWORDVec		m_dwResultMarkerIDList;

	ioHashString	m_szSkillLoopMotion;
	CEncrypt<float>	m_fSkillLoopMotionRate;
	CEncrypt<DWORD>	m_dwSkillLoopDuration;

	ioHashString m_SkillMotionFr;
	ioHashString m_SkillMotionBk;
	ioHashString m_SkillMotionLt;
	ioHashString m_SkillMotionRt;

	float m_fSkillMotionRate;

	WeaponInfo m_WeaponInfo;
	WeaponFireInfoList m_vWeaponFireInfoList;
	int m_iWeaponFireCnt;

	bool m_bMotionSetted;
	bool m_bLoopMotionSetted;
	bool m_bFirstMoveMotion;
	ioUserKeyInput::DirKeyInput m_StartDirKey;

protected:
	void ClearData();

public:	// overide ioSkill
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillLoopState( ioBaseChar *pOwner );
	void ProcessAttack( ioBaseChar *pOwner );
	void CheckMoveAniState( ioBaseChar *pOwner );

public:
	void CreateWeapon( ioBaseChar *pOwner );

public:
	ioConsecution2Skill();
	ioConsecution2Skill( const ioConsecution2Skill &rhs );
	virtual ~ioConsecution2Skill();
};

inline ioConsecution2Skill* ToConsecution2Skill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CONSECUTION2 )
		return NULL;

	return dynamic_cast< ioConsecution2Skill* >( pAttack );
}

