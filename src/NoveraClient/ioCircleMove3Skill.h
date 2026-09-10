#pragma once


#include "ioNormalSkill.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMove3Skill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_LOOP,
		SS_ACTION,		
		SS_END,
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

	UniqueObjID m_dwTargetCircleEffect;

	CEncrypt<float>	m_fEndMotionRate;
	ioHashString	m_SkillEndMotion;

	ioHashString	m_LoopAnimation;
	CEncrypt<DWORD>	m_dwLoopTime;
	DWORD			m_dwLoopStartTime;

	CEncrypt<DWORD>	m_dwLoopProtectDuration;
	DWORD			m_dwProtectTime;

	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	ioHashString	m_szRangeEffect;
	CEncrypt<float>	m_fSearchRadius;
	CEncrypt<float>	m_fSearchHeight;

	DWORD			m_dwNextFireTime;
	Vector3Vec		m_vFirePos;

	//AreaWeapon
	ioHashString m_AreaWeaponName;

	//¾Ç·ÉµµµÏ¶§ Ãß°¡
	bool			m_bNoneStopActive;

	DWORD m_dwAreaWeaponIndex;

protected:
	void ClearData();

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

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	//virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetWaitState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	void SearchTarget( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckCreateWeapon( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioCircleMove3Skill();
	ioCircleMove3Skill( const ioCircleMove3Skill &rhs );
	virtual ~ioCircleMove3Skill();
};

inline ioCircleMove3Skill* ToCircleMove3Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE3 )
		return NULL;

	return dynamic_cast< ioCircleMove3Skill* >( pSkill );
}

