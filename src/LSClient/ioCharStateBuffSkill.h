#pragma once
#include "iobuffskill.h"

class ioBaseChar;
class ioINILoader;
class ioBuff;

class ioCharStateBuffSkill :	public ioBuffSkill
{
protected:
	BuffInfoList m_OwnerWoundedBuffList;

	bool	m_bWounded;

	BlowWoundedState	m_CharBlowWoundedState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	//virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	//virtual bool IsCanUseSkillWithCrown( ioBaseChar *pChar );

	virtual bool IsOnOffEnable();
	virtual bool OnSkillStart( ioBaseChar *pChar );
	//virtual void OnSkillEnd( ioBaseChar *pChar );	

	//virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	//virtual void OnProcessNotUseSkill( ioBaseChar *pChar, ioPlayStage *pStage );
	//virtual void OnSkillItemRelease( ioBaseChar *pChar );
	//virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void BuffSkillSet( ioBaseChar *pChar );

	virtual bool HasBuff( int iBuffType ) const;
	virtual int GetBuffType( int iBuffType = 0 );

	virtual bool CheckUseBuff( ioBaseChar *pChar );
	virtual bool CheckUseItemMesh();

	virtual bool IsEndState() const;

protected:
	//void LoadBuffList( ioINILoader &rkLoader );
	bool CheckEnableStart( ioBaseChar *pChar );

	bool IsCanShowWeaponItemMesh( ioBaseChar *pChar );

public:
	virtual void RemoveSkillBuff( ioBaseChar *pChar );
	virtual void RemoveSkillBuff( ioBaseChar *pChar, const ioHashString &szExceptName );
	virtual void RemoveAreaWeapon( ioBaseChar *pChar );

	//virtual DWORD GetCreateItemCode( ioBaseChar *pChar );

	bool CheckUseAreaWeapon();

	void AddOwnerBuff( ioBaseChar *pChar );
	//void AddTargetBuff( ioBaseChar *pChar );
	//void CreateAreaWeapon( ioBaseChar *pChar );

	//void CheckUseBuffMotionEnd( ioBaseChar *pChar );

	bool HasOwnerBuff( const ioHashString &szBuffName );

	inline bool IsCanStateEscape() const { return m_bStateEscape; }
	inline bool IsUseBuffMotionEnd() const { return m_bUseBuffMotionEnd; }
	inline int GetCurSkillState() const { return m_SkillState; }

	//void CheckRemoveEquipSkillBuff( bool bRemoveEquipSkillBuff );

	inline bool IsBlowWounded() { return m_bWounded; }
	inline BlowWoundedState GetPreBlowWoundedState() { return m_CharBlowWoundedState; }

public:
	ioCharStateBuffSkill(void);
	ioCharStateBuffSkill( const ioCharStateBuffSkill& rhs );
	virtual ~ioCharStateBuffSkill(void);
};

inline ioCharStateBuffSkill* ToCharStateBuffSkill( ioSkill *pSkill )
{
	ioBuffSkill *pBuff = ToBuffSkill( pSkill );
	if( !pBuff || pBuff->GetSubSkillType() != BST_CHAR_STATE )
		return NULL;

	return dynamic_cast< ioCharStateBuffSkill* >( pSkill );
}