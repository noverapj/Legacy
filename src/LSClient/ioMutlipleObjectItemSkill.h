#pragma once

#include "ioNormalSkill.h"
#include "WeaponAttribute.h"

class ioBaseChar;
class ioINILoader;

class ioMultipleObjectItemSkill : public ioNormalSkill
{
public:
	enum CreateState
	{
		RS_NONE,
		RS_ACTION,
		RS_LOOP,
		RS_END,
	};

protected:
	CreateState m_CreateState;

	ioHashString m_LoopAnimation;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopTime;

	DWORD m_dwLoopProtectDuration;
	DWORD m_dwProtectTime;

	ioHashString m_EndAnimation;
	float m_fEndAniRate;
	DWORD m_dwEndAniStartTime;
	DWORD m_dwEndAniEndTime;

	bool m_bImmediatelyEquip;
	ioHashString m_StartHandMesh;

protected:
	float m_fOffSet;

	ioHashString m_CreateEffect;
	DWORDVec     m_vObjectItemList;

	BuffInfoList m_OwnerBuffList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

	void LoadOwnerBuffList( ioINILoader &rkLoader );

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

	virtual bool CheckUseItemMesh();

protected:
	void SetActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar, ioPlayStage* pStage );
	bool SetOwnerBuffList( ioBaseChar *pChar, ioPlayStage* pStage );

public:
	ioMultipleObjectItemSkill();
	ioMultipleObjectItemSkill( const ioMultipleObjectItemSkill &rhs );
	virtual ~ioMultipleObjectItemSkill();
};

inline ioMultipleObjectItemSkill* ToMultipleObjectItemSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_MULTIPLE_OBJECT )
		return NULL;

	return dynamic_cast< ioMultipleObjectItemSkill* >( pSkill );
}

