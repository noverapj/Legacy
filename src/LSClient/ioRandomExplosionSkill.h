#pragma once
#include "ioAttackSkill.h"

class ioRandomExplosionSkill :	public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_END,
	};

	enum SyncType
	{
		ST_CREATE_WEAPON,
		ST_CREATE_ALL_WEAPON,
		ST_END
	};

	enum CreateWeaponType
	{
		CWT_ONE,
		CWT_ALL,
	};
protected:
	SkillState					m_SkillState;
	CreateWeaponType			m_CreateWeaponType;
	// areaweapon
	ioHashString				m_AreaWeaponName;
	DWORD						m_dwAreaWeaponIndex;

	//Loop
	ioHashString				m_szLoopAni;
	float						m_fLoopAniRate;
	DWORD						m_dwLoopDuration;

	DWORD						m_dwKeyCheckStartTime;
	bool						m_bAddKey;

	DWORD						m_dwKeyAddDuration;
	DWORD						m_dwKeyEnableTime;

	float						m_fCollisionRange;

	WeaponInfoList				m_vWeaponInfoList;
	int							m_iCurWeaponCount;

	//protect
	DWORD						m_dwSkillProtectStartTime;
	DWORD						m_dwSkillProtectTime;

	WeaponInfoList				m_vLoopStartWeaponList;
	std::vector<D3DXVECTOR3>	m_vLoopStartWeaponPositionList;
protected:
	void ClearData();

	void CreateAreaWeapon( ioBaseChar *pChar );
	void CreateRandomPosWeapon( ioBaseChar *pChar );
	void CreateRandomPosAllWeapon( ioBaseChar *pChar );

	void SetLoopState( ioBaseChar *pChar );

	void SetEndState( ioBaseChar *pChar );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;
public:
	ioRandomExplosionSkill(void);
	ioRandomExplosionSkill( const ioRandomExplosionSkill &rhs );
	~ioRandomExplosionSkill(void);
};

inline ioRandomExplosionSkill* ToRandomExplosionSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RANDOM_EXPLOSION )
		return NULL;

	return dynamic_cast< ioRandomExplosionSkill* >(pAttack);
}