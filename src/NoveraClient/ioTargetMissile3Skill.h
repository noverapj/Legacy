#pragma once

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTargetMissile3Skill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_FIRE,
		SS_END,
	};

	enum SyncState
	{
		SYNC_SET_NORMAL_SKILL,
		SYNC_FIRE,
		SYNC_END,
	};

	struct TargetInfo
	{
		ioHashString name;
		float distance;
		bool is_floating;
	};
	typedef std::vector<TargetInfo> vTargetInfo;

protected:
	SkillState m_State;

	// Attack
	DWORD m_dwFireStartTime;

	WeaponInfo m_WeaponInfo;
	WeaponInfo m_WeaponInfoAir;

	ioHashString m_szLoopAnimation;
	CEncrypt<float> m_fLoopAniRate;

	// Circle
	CEncrypt<float> m_fCircleRange;
	CEncrypt<float> m_fCircleMoveSpeed;
	CEncrypt<float> m_fCircleHeightGap;
	CEncrypt<float> m_fCircleOffSet;

	CEncrypt<float> m_fHeightOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;

	DWORD m_dwActionStartTime;

	CEncrypt<float> m_fTargetRange;
	TargetWoundType m_TargetWoundType;

	vTargetInfo m_vTargetList;

	CEncrypt<float> m_fGuagePerTic;
	CEncrypt<DWORD> m_dwTicTime;

	DWORD m_dwCheckTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

public:
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

private:
	void SetWaitState( ioBaseChar *pChar );
	void SetFireState( ioBaseChar *pChar );

	void CheckTarget( ioBaseChar *pChar );
	bool IsEnableTargetState( ioBaseChar *pTarget );

public:
	ioTargetMissile3Skill();
	ioTargetMissile3Skill( const ioTargetMissile3Skill &rhs );
	virtual ~ioTargetMissile3Skill();
};

inline ioTargetMissile3Skill* ToTargetMissile3Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_MISSILE3 )
		return NULL;

	return dynamic_cast< ioTargetMissile3Skill* >( pSkill );
}

