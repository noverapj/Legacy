#pragma once


#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;
class ioMapEffect;

class ioCannonChargeSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE,
		SS_LOOP,
		SS_FIRE,
		SS_EXPLOSION,
		SS_END,
	};

	enum SkillSync
	{
		SST_LOOP,
		SST_FIRE,
	};

	enum LoopSync
	{
		SLT_LOOP,
		SLT_FIRE,
	};

protected:
	CEncrypt<int> m_State;

	//AttackAttribute
	AttackAttribute m_AttackAttribute;

	DWORD m_DwReadyActionEndTime;

	// Attack
	ioHashString m_SkillLoopMotion;
	ioHashString m_SkillPreMotion;

	float m_fSkillPreMotionLate;
	float m_fSkillLoopMotionRate;
	
	float m_fCharTriggerHeight;

	float m_fCurMoveSpeed;
	float m_fSkillMoveRange;
	
	DWORD m_dwLoopDurationStart;
	DWORD m_dwLoopDurationEnd;
	DWORD m_dwOrgLoopDuration;
	
	DWORD m_dwSkillFireRotateTime;
	DWORD m_dwSkillLoopRotateTime;

	bool m_bLeftRot;
	bool m_bSend;

	float m_fSkillTotalMoveAmount;

	//이동 중 무적이 적용될 사거리
	float m_fMoveProtectedRange;	

	BuffInfoList m_OwnerBuffList;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	//
	ioHashString m_ReadySkillMapEffect;
	UniqueObjID m_dwReadySkillMapEffect;
	
	ioHashString m_LoopSkillMapEffect;
	UniqueObjID m_dwLoopSkillMapEffect;
	
	ioHashString m_FireSkillMapEffect;
	UniqueObjID m_dwFireSkillMapEffect;

	D3DXVECTOR3 m_vSkillStartPos;

	ioHashString m_CannonFireSound;
	UniqueObjID m_CannonFireSoundID;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );	
	virtual ioSkill* Clone();
	
	virtual SkillCharColType IsCharSkipState() const;

	void LoadBuffList( ioINILoader &rkLoader );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();

	void UpdateEffect( ioBaseChar *pChar, ioPlayStage *pStage );	

public:
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsCanSendControl() const;
	
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock );

	bool CheckWeaponDead( ioBaseChar *pChar );
	
protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );

protected:	
	void SetReadyActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void AttackSkillFire( ioBaseChar *pChar );

	void CheckLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckFireState( ioBaseChar *pChar );
	
	void SetExplosionState(ioBaseChar *pChar);

	void SetEndState( ioBaseChar *pChar );

	void CheckKeyInput( ioBaseChar *pChar );
	void ProcessRotate( ioBaseChar *pChar, float fRotateTime );

public:
	ioCannonChargeSkill();
	ioCannonChargeSkill( const ioCannonChargeSkill &rhs );
	virtual ~ioCannonChargeSkill();
};

inline ioCannonChargeSkill* ToCannonChargeSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CANNON_CHARGE )
		return NULL;

	return dynamic_cast< ioCannonChargeSkill* >( pSkill );
}

