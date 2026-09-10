#pragma once

#include "ioNormalSkill.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioMultiArea2Skill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};

	enum SkillSyncState
	{
		SSS_KEY,
		SSS_ACTION,
	};

protected:
	CEncrypt<int> m_State;

	CEncrypt<bool>	m_bDirectFire;
	Vector3Vec		m_vAreaWeaponPosList;

	// areaweapon
	ioHashString m_AreaWeaponName;

	CEncrypt<int> m_iMaxAreaWeaponCnt;
	CEncrypt<int> m_iCurAreaWeaponCnt;

	ioHashString	m_szEnableCircle;
	DWORDVec		m_dwEnableCircleIDList;

	CEncrypt<DWORD>	m_dwEnableKeyTime;
	DWORD			m_dwGapTime;

	DWORD m_dwFireStartTime;
	DWORD m_dwActionStartTime;

	// magic circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;

	ioHashString m_RedGuidCircle;
	ioHashString m_BlueGuidCircle;

	//
	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	ioHashString	m_LoopAnimation;
	CEncrypt<DWORD>	m_dwLoopProtectTime;
	DWORD			m_dwLoopStartTime;

	//
	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

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
	void CheckCreateAreaWeapon( ioBaseChar *pChar );

	void SetMagicCircle( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );
	void AddAreaPosition( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioMultiArea2Skill();
	ioMultiArea2Skill( const ioMultiArea2Skill &rhs );
	virtual ~ioMultiArea2Skill();
};

inline ioMultiArea2Skill* ToMultiArea2Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_MULTI_AREA2 )
		return NULL;

	return dynamic_cast< ioMultiArea2Skill* >( pSkill );
}

