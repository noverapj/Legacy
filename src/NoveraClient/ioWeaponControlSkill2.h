#pragma once

#include "ioNormalSkill.h"


class ioWeaponControlSkill2 : public ioNormalSkill
{
public:
	enum ControlState
	{
		RS_NONE,
		RS_ACTION,
		RS_LOOP,
		RS_END,
	};

	enum SyncState
	{
		SS_CREATE,
	};

protected:
	ControlState m_ControlState;

	DWORD m_dwLoopProtectDuration;
	DWORD m_dwProtectTime;

	AttackAttribute m_AttackAttribute;

	D3DXVECTOR3 m_vCreatePos;	
	DWORD m_dwFireStartTime;

	// Animation
	ioHashString m_LoopAnimation;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopTime;

	ioHashString m_EndAnimation;
	float m_fEndAniRate;
	DWORD m_dwEndAniEndTime;

	// HandMesh
	ioHashString m_StartHandMesh;
	ioHashString m_LoopHandMesh;
	ioHashString m_EndHandMesh;

	// 스킬 게이지 감소 관련
	CEncrypt<bool>	m_bDisableGrowth;	
	bool			m_bReduceSkillGuage;
	CEncrypt<float>	m_fGuagePerTic;
	CEncrypt<DWORD>	m_dwTicTime;
	DWORD			m_dwCheckTime;
	float			m_fTicGaugeRateByGrowth;
	DWORD			m_dwTicTimeByGrowth;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void StartPreDelayEffect( ioBaseChar *pChar );
	virtual void EndPreDelayEffect( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

protected:
	void ClearData();

protected:
	void SetActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	bool CheckControlWeapon( ioBaseChar *pChar );

public:
	bool IsWeaponControl( ioBaseChar *pChar );
	const D3DXVECTOR3& GetWeaponPos( ioBaseChar *pChar );

public:
	ioWeaponControlSkill2();
	ioWeaponControlSkill2( const ioWeaponControlSkill2 &rhs );
	virtual ~ioWeaponControlSkill2();
};

inline ioWeaponControlSkill2* ToWeaponControlSkill2( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_WEAPON_CONTROL2 )
		return NULL;

	return dynamic_cast< ioWeaponControlSkill2* >( pSkill );
}

