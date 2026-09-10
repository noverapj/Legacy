#pragma once

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioChargingFireSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_ACTION,
		SS_LOOP,
		SS_ACTION,
	};

	enum SkillSync
	{
		SSC_PRE_ACTION,
		SSC_LOOP,
		SSC_ACTION,
	};

protected:
	CEncrypt<int> m_State;

	// Attack
	AttackAttribute m_AttackAttribute;

	ioHashString m_SkillLoopMotion;
	float m_fSkillLoopMotionRate;

	DWORD m_dwMaxChargingTime;
	DWORD m_dwFireStartTime;

	// Char
	D3DXVECTOR3 m_vCharMoveDir;
	float m_fCharMoveSpeed;
	float m_fCharJumpPower;

	DWORD m_dwPreActionEndTime;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;

	DWORD m_dwAttackEndTime;

	ioHashString m_SkillChargingEffect;
	UniqueObjID m_dwChargingEffect;

	// Circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;
	ioHashString m_TargetCircleEffect;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	UniqueObjID m_dwTargetCircleEffect;

	//
	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	//
	ioUserKeyInput::DirKeyInput m_CurDirKey;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;

	virtual bool IsNoDropState() const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetPreActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend );

	void ProcessLoopState(  ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessPreAction(  ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckCharMoveDir( ioBaseChar *pChar );

public:
	ioChargingFireSkill();
	ioChargingFireSkill( const ioChargingFireSkill &rhs );
	virtual ~ioChargingFireSkill();
};

inline ioChargingFireSkill* ToChargingFireSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CHARGING_FIRE )
		return NULL;

	return dynamic_cast< ioChargingFireSkill* >( pSkill );
}

