#pragma once
#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioMosesSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_LOOP,
		SS_END,
	};	
protected:
	SkillState m_SkillState;
	
	DWORD m_dwWeaponIdx;
	
	float m_fZoneRange;
	float m_fZoneWidth;

	float m_fZoneRangeTemp;
	float m_fZoneWidthTemp;

	float m_fRangeMax;
	float m_fwidthMax;

	CEncrypt<float> m_fScaleRate;
	CEncrypt<float> m_fMaxScaleRate;
	float m_fScaleRateTemp;

	float m_fAreaRangeTemp;
	float m_fAreaWidthTemp;

	float m_fAreaRange;
	float m_fAreaWidth;

	float m_fAreaRangeRate;
	float m_fAreaWidthRate;

	DWORD m_dwFireStartTime;
	
	//key
	bool m_bAddKey;
	CEncrypt<DWORD> m_dwAddDuration;
	CEncrypt<DWORD> m_dwEnableKeyTime;
	DWORD m_dwCheckStartTime;
	CEncrypt<DWORD> m_dwAddDurationMax;
	CEncrypt<DWORD> m_dwEnableKeyTimeMax;

	ioHashString m_AreaWeaponName;
	DWORD m_dwCreatedAreaWeaponIndex;

	D3DXVECTOR3 m_vAttackDir;

	// 스킬 게이지 감소
	CEncrypt<bool> m_bDisableGrowth;
	CEncrypt<float> m_fGuagePerTic;
	CEncrypt<DWORD> m_dwTicTime;

	DWORD m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	int m_iCurSkillAniID;
	float m_fTimeRate;

	CEncrypt<DWORD> m_dwSkillProtectTime;
	bool m_bCheckEffctScale;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsProtected( int iDefenseBreakType ) const;
public:
	void ClearData();
	void SetLoopState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );
	void CreateAreaWeapon( ioBaseChar* pChar, const D3DXVECTOR3& vPos );
	void CreateZoneEffect( ioBaseChar* pChar );
	ioAreaWeapon* FindAreaWeapon( ioBaseChar *pChar );

	void DestroyAreaWeapon( ioBaseChar *pChar );
	
public:
	ioMosesSkill(void);
	ioMosesSkill( const ioMosesSkill& rhs );
	virtual ~ioMosesSkill(void);
};


inline ioMosesSkill* ToMosesSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_MOSESS )
		return NULL;

	return dynamic_cast< ioMosesSkill* >( pAttack );
}



