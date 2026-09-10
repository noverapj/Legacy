#pragma once
#include "ioattackskill.h"
class ioCreateTicWeaponSkill : public ioAttackSkill
{
private:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_FIRE,
		SS_END,
	};

	enum SkillSyncType
	{
		SST_CRATED_TIC_EFFECT,
		SST_FIRE,
	};

	struct WeaponEffect
	{
		DWORD		dwEffectID;
		D3DXVECTOR3 vOffset;
	};

	typedef std::vector<WeaponEffect> WeaponEffectList;
private:
	SkillState			m_SkillState;

	//Effect
	ioHashString		m_szWeaponEffectName;
	Vector3Vec			m_vStartEffectOffsetList;
	WeaponEffectList	m_vCurWeaponEffectList;
	DWORD				m_vNextTicEffectCreateTime;
	DWORD				m_dwTicEffectCreateTicTime;
	int					m_iTicEffectCurCount;
	Vector3Vec			m_vTicEffectOffsetList;
	

	//Loop
	bool				m_bInputAttackKey;
	ioHashString		m_LoopDelayAnimation;
	float				m_fLoopDelayAniRate;
	ioHashString		m_LoopMoveAnimation;
	float				m_fLoopMoveAniRate;
	bool				m_bSetMoveAni;
	DWORD				m_dwLoopStartTime;
	DWORD				m_dwLoopDuration;
	DWORD				m_dwLoopProtectEndTime;
	DWORD				m_dwLoopProtectTime;

	//Fire
	DWORD				m_dwFireStartTime;

	//Weapon
	WeaponInfoList		m_vWeaponInfoList;


	BuffInfoList m_OwnerBuffList;

private:
	void ClearData();

	void SetLoopState( ioBaseChar *pOwner );
	void SetFireState( ioBaseChar *pOwner );

	void CheckNormalMoveAni( ioBaseChar* pOwner );
	void CreateTicWeapon( ioBaseChar* pOwner );

	void CreateStartCallWeapon( ioBaseChar* pOwner  );
	void UpdateWeaponEffect( ioBaseChar *pOwner, ioPlayStage *pStage );
	
	void FireEffectWeapon( ioBaseChar* pOwner, ioPlayStage *pStage );

	void LoadBuffList( ioINILoader &rkLoader );

	void AddOwnerBuff( ioBaseChar *pOwner );
	void EraseOwnerBuff( ioBaseChar *pOwner );
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pOwner );
	virtual void OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage );
	virtual void AttackSkillFire( ioBaseChar *pOwner );
	virtual void OnSkillEnd( ioBaseChar *pOwner );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket );
	//virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	//virtual bool IsAttackEndState() const;
public:
	ioCreateTicWeaponSkill(void);
	ioCreateTicWeaponSkill( const ioCreateTicWeaponSkill &rhs );
	~ioCreateTicWeaponSkill(void);
};

inline ioCreateTicWeaponSkill* ToCreateTicWeaponSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CREATE_TIC_WEAPON_ATTACK )
		return NULL;

	return dynamic_cast< ioCreateTicWeaponSkill* >( pAttack );
}