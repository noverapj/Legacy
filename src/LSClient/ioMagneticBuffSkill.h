#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioBuffSkill.h"
#include "ioUserKeyInput.h"

class ioMagneticBuffSkill : public ioBuffSkill
{
protected:
	int			m_iCurBuff;			// N극 = 1, S극 = 0 
	bool		m_bDefenseKey;

	DWORD		m_dwDefenseStartCoolTime;	// 방어키 입력 시간 2020-12-17
	DWORD		m_dwDefenseCoolTime;		// 방어키 조절 2020-12-17

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
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

	// 2020-12-09
	void CheckKeyInput( ioBaseChar *pOwner );

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
	ioMagneticBuffSkill();
	ioMagneticBuffSkill( const ioMagneticBuffSkill &rhs );
	virtual ~ioMagneticBuffSkill();
};

inline ioMagneticBuffSkill* ToMagneticBuffSkill( ioSkill *pSkill )
{
	ioBuffSkill *pBuff = ToBuffSkill( pSkill );
	if( !pBuff || pBuff->GetSubSkillType() != BST_MAGNETIC )
		return NULL;

	return dynamic_cast< ioMagneticBuffSkill* >( pSkill );
}

