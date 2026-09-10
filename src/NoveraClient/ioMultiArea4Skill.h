#pragma once

#include "ioNormalSkill.h"
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioMultiArea4Skill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_TELEPORT,
		SS_END,
	};

	enum SkillSyncState
	{
		SSC_ACTION,
		SSC_TELEPORT,
	};

	struct SkillMotion
	{
		ioHashString	m_Motion;
		float			m_fMotionRate;
		DWORD			m_dwMotionEndDelay;
		SkillMotion()
		{
			Init();
		}
		void Init()
		{
			m_Motion.Clear();
			m_fMotionRate = FLOAT1;
			m_dwMotionEndDelay = 0.0f;
		}
	};
	typedef std::vector<SkillMotion>	SkillMotionList;

protected:
	CEncrypt<int> m_State;
	SkillMotionList	m_SkillMotionList;

	WeaponInfoList	m_WeaponInfoList;

	bool			m_bCheckDropZone;

	DWORD m_dwFireStartTime;
	DWORD m_dwActionStartTime;

	// team circle
	ioHashString	m_RedCircle;
	ioHashString	m_BlueCircle;
	UniqueObjID		m_dwCurMapEffect;

	// magic circle
	float m_fCircleMaxRange;
	float m_fCircleMinRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vStartPos;
	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;
	ioHashString m_RedGuidCircle;
	ioHashString m_BlueGuidCircle;

	// Protect time
	CEncrypt<DWORD>	m_dwSkillProtectTime;

	// Loop Motion
	ioHashString	m_SkillLoopMotion;
	DWORD			m_dwSkillLoopStartTime;
	CEncrypt<DWORD>	m_dwSkillLoopDuration;

	// End Motion
	ioHashString m_SkillEndMotion;
	float m_fEndMotionRate;

	// Combo & Key input
	CEncrypt<int>	m_iMaxCombo;
	int				m_iCurCombo;

	CEncrypt<DWORD>	m_dwEnableKeyTime;
	CEncrypt<DWORD>	m_dwEnableTeleportTime;
	DWORD			m_dwEnableNextTime;

	// Teleport effect
	ioHashString	m_TeleportStartEffect;
	ioHashString	m_TeleportEndEffect;

	BuffInfoList m_OwnerBuffList;
	
	float m_fCollisionRange;
protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	void LoadOwnerBuffList( ioINILoader &rkLoader );

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
	void CreateMagicCircle( ioBaseChar *pChar );
	void UpdateMagicCircle( ioBaseChar *pChar );
	void EndMagicCircle( ioBaseChar *pChar );
	void DestroyMagicCircle( ioBaseChar *pChar );

	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetSkillTeleportState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar );

	void AddOwnerBuff( ioBaseChar *pChar );
	void RemoveOwnerBuff( ioBaseChar *pChar );
	void RandomPosAttack( ioBaseChar *pChar );
public:
	ioMultiArea4Skill();
	ioMultiArea4Skill( const ioMultiArea4Skill &rhs );
	virtual ~ioMultiArea4Skill();
};

inline ioMultiArea4Skill* ToMultiArea4Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_MULTI_AREA4 )
		return NULL;

	return dynamic_cast< ioMultiArea4Skill* >( pSkill );
}

