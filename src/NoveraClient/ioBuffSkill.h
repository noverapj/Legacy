#pragma once

#include "ioSkill.h"
#include "WeaponAttribute.h"

class ioBaseChar;
class ioINILoader;
class ioBuff;

class ioBuffSkill : public ioSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_END,
	};

protected:
	SkillState m_SkillState;

	BuffInfoList m_TargetBuffList;
	BuffInfoList m_OwnerBuffList;
	BuffInfoList m_EnemyBuffList;

	ForceInfoList m_vForceInfoList;

	ioHashString m_AreaWeaponName;

	DWORD m_dwAreaWeaponIndex;
	
	bool	m_bNeedTarget;
	bool	m_bOnOffEnable;
	CEncrypt<DWORD>	m_dwOnOffEnableTime;
	bool	m_bUseBuffMotionEnd;

	bool	m_bStateEscape;
	bool	m_bSkillMoveEnableDelayTime;

	DWORD	m_dwAttackEndTime;

	bool	m_bCanEnableBuffState;

protected:
	DWORD m_dwFireStartTime;

	bool	m_bRemoveEquipSkillBuff;

protected:
	ioHashString	m_SkillActivateEffect;
	DWORD			m_SkillActivateEffectID;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	virtual int GetTypeID() const;

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	//virtual bool IsCanUseSkillWithCrown( ioBaseChar *pChar );

	virtual bool IsOnOffEnable();
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	

	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnProcessNotUseSkill( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillItemRelease( ioBaseChar *pChar );
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void BuffSkillSet( ioBaseChar *pChar );

	virtual bool HasBuff( int iBuffType ) const;
	virtual int GetBuffType( int iBuffType = 0 );

	virtual bool CheckUseBuff( ioBaseChar *pChar );
	virtual bool CheckUseItemMesh();

	virtual bool IsEndState() const;

protected:
	void LoadBuffList( ioINILoader &rkLoader );
	bool CheckEnableStart( ioBaseChar *pChar );

	bool IsCanShowWeaponItemMesh( ioBaseChar *pChar );

public:
	virtual void RemoveSkillBuff( ioBaseChar *pChar );
	virtual void RemoveSkillBuff( ioBaseChar *pChar, const ioHashString &szExceptName );
	virtual void RemoveAreaWeapon( ioBaseChar *pChar );

	virtual DWORD GetCreateItemCode( ioBaseChar *pChar );

	bool CheckUseAreaWeapon();

	void AddOwnerBuff( ioBaseChar *pChar );
	void AddTargetBuff( ioBaseChar *pChar );
	void CreateAreaWeapon( ioBaseChar *pChar );

	void CheckUseBuffMotionEnd( ioBaseChar *pChar );
	
	bool HasOwnerBuff( const ioHashString &szBuffName );

	inline bool IsCanStateEscape() const { return m_bStateEscape; }
	inline bool IsUseBuffMotionEnd() const { return m_bUseBuffMotionEnd; }
	inline int GetCurSkillState() const { return m_SkillState; }

	void CheckRemoveEquipSkillBuff( bool bRemoveEquipSkillBuff );



public:
	ioBuffSkill();
	ioBuffSkill( const ioBuffSkill &rhs );
	virtual ~ioBuffSkill();
};

inline ioBuffSkill* ToBuffSkill( ioSkill *pSkill )
{
	if( !pSkill || pSkill->GetTypeID() != ST_BUFF )
		return NULL;

	return dynamic_cast< ioBuffSkill* >( pSkill );
}

