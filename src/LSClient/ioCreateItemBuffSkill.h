#pragma once

#include "ioBuffSkill.h"

class ioBaseChar;
class ioINILoader;
class ioBuff;

class ioCreateItemBuffSkill : public ioBuffSkill
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
	bool  m_bUseClearGauge;

protected:
	ioHashString m_StartHandMesh;

	ioHashString m_CreateEffect;
	DWORDVec     m_vObjectItemList;

	DWORD m_dwCurObjectItem;

	CEncrypt<bool>	m_bProtectOnUsingSkill;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void BuffSkillSet( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void RemoveSkillBuff( ioBaseChar *pChar );
	virtual void RemoveSkillBuff( ioBaseChar *pChar, const ioHashString &szExceptName );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEndState() const;

	virtual DWORD GetCreateItemCode( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

public:
	ioCreateItemBuffSkill();
	ioCreateItemBuffSkill( const ioCreateItemBuffSkill &rhs );
	virtual ~ioCreateItemBuffSkill();
};

inline ioCreateItemBuffSkill* ToCreateItemBuffSkill( ioSkill *pSkill )
{
	ioBuffSkill *pBuff = ToBuffSkill( pSkill );
	if( !pBuff || pBuff->GetSubSkillType() != BST_CREATE_ITEM )
		return NULL;

	return dynamic_cast< ioCreateItemBuffSkill* >( pSkill );
}

