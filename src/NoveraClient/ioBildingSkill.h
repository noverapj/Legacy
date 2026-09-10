#pragma once


#include "ioNormalSkill.h"
#include "WeaponAttribute.h"

class ioBaseChar;
class ioINILoader;

class ioBildingSkill : public ioNormalSkill
{
public:
	enum BilgindState
	{
		RS_NONE,
		RS_ACTION,
		RS_LOOP,
		RS_END,
	};
	
protected:
	BilgindState m_BildingState;

	ioHashString m_LoopAnimation;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopTime;

	DWORD m_dwLoopProtectDuration;
	DWORD m_dwProtectTime;

	ioHashString m_EndAnimation;
	float m_fEndAniRate;
	DWORD m_dwEndAniStartTime;
	DWORD m_dwEndAniEndTime;

protected:
	ioHashString m_StartHandMesh;
	ioHashString m_LoopHandMesh;
	ioHashString m_EndHandMesh;

	float m_fOffSet;
	int m_iBildStruct;
	int m_iMaxStructCnt;

	ioHashString m_BildEffect;

	bool m_bEffectCreated;

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

	virtual bool CheckUseItemMesh();

protected:
	void SetActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	D3DXVECTOR3 GetCreatePosition( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioBildingSkill();
	ioBildingSkill( const ioBildingSkill &rhs );
	virtual ~ioBildingSkill();
};

inline ioBildingSkill* ToBildingSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_BILDING )
		return NULL;

	return dynamic_cast< ioBildingSkill* >( pSkill );
}

