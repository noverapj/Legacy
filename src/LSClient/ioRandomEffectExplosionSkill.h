#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioRandomEffectExplosionSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_LOOP,
		SS_FINISH,
		SS_END,
	};

protected:
	SkillState m_SkillState;

	ioHashString m_szLoopAni;
	DWORD m_dwLoopDuration;
	DWORD m_dwStartLoopTime;

	DWORD m_dwKeyCheckStartTime;
	CEncrypt<DWORD>	m_dwKeyAddDuration;
	CEncrypt<DWORD>	m_dwKeyEnableTime;

	bool m_bAddKey;

	ioHashString m_szSkillWeaponEffect;

	std::vector<UniqueObjID> m_vWeaponEffectIDList;

	float m_fCollisionRange;
	int m_iCurCombo;
	
	std::vector<D3DXVECTOR3> m_vEffectPosList;

	WeaponInfo m_WeaponInfo;
	DWORD m_dwFireStartTime;

	// areaweapon
	ioHashString m_AreaWeaponName;
	DWORD m_dwAreaWeaponIndex;

	bool m_bEnd;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	void ClearData();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;

protected:
	void SetLoopState( ioBaseChar *pChar );
	void SetFinishState( ioBaseChar *pChar );

protected:
	void RandomPosCreateEffect( ioBaseChar *pChar );
	void CreateEffect( ioBaseChar *pChar, D3DXVECTOR3 vPos );
	void DestroyEffect( ioBaseChar *pChar );

	void EffectExplosion( ioBaseChar* pChar );
	// areaweapon
	void CreateAreaWeapon( ioBaseChar *pChar );
public:
	ioRandomEffectExplosionSkill(void);
	ioRandomEffectExplosionSkill( const ioRandomEffectExplosionSkill &rhs );
	virtual ~ioRandomEffectExplosionSkill();
};


inline ioRandomEffectExplosionSkill* ToRandomEffectExplosionSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RANDOM_EFFECT_EXPLOSION )
		return NULL;

	return dynamic_cast< ioRandomEffectExplosionSkill* >(pAttack);
}

