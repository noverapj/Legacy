#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioTwoStateAttack : public ioAttackSkill
{
protected:

	enum SkillState
	{
		SS_PRE,
		SS_JUMP_PRE,
		SS_LOOP,		
		SS_ACTION,
		SS_AIR_LOOP,
		SS_AIR_ACTION,
		SS_END,
	};

	enum SkillSync
	{	
		SSC_LOOP,
		SSC_AIR_LOOP,
		SSC_ACTION,
		SSC_AIR_ACTION,
	};

protected:

	SkillState m_State;

	//Loop
	ioHashString m_SkillLoopMotion;
	ioHashString m_SkillAirCenterLoopMotion;
	ioHashString m_SkillAirDownLoopMotion;
	ioHashString m_skillAirUpLoopMotion;

	float m_fSkillLoopMotionRate;
	float m_fSkillAirLoopMotionRate;
			
	DWORD m_dwSkillLoopDuration;
	DWORD m_dwSkillAirLoopDuration;

	DWORD m_dwSkillLoopProtectTime;
	DWORD m_dwSkillAirLoopProtectTime;
	DWORD m_dwSkillLoopProtectEndTime;	

	DWORD m_dwLoopEndTime;
	DWORD m_dwSkillLoopRotateSpeed;

	//Jumping
	DWORD m_dwJumpMotionEndTime;

	//Air Action
	ioHashString m_SkillAirCenterMotion;
	ioHashString m_SkillAirDownMotion;
	ioHashString m_SkillAirUpMotion;
	float m_fAirAnimationTimeRate;
	DWORD m_dwAirPreDelay;
	DWORD m_dwAirEndDelay;
	WeaponInfoList m_vWeaponInfoList;

	float m_fCurMotionRate;

	//magic circle	
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_CircleEffect;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	//target guid
	ioHashString m_ExtraGuidEffect;
	UniqueObjID m_dwExtraGuidEffect;

	ioHashString m_FireGuidEffect;
	UniqueObjID m_dwFireGuidEffect;

	ioHashString m_SkillMapEffect;
	UniqueObjID m_dwSkillMapEffect;

	ioHashString m_SkillMapAirEffect;
	UniqueObjID m_dwSkillMapAirEffect;

	float m_fSkillMapOffset;
	D3DXVECTOR3 m_vAttackDir;

	D3DXVECTOR3 m_vMagicCirclePos;
	D3DXVECTOR3 m_vAttackMoitonDir;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	float m_fCharJumpPower;
	bool m_bAirState;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	
	virtual bool IsNoDropState() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool IsAttackEndState() const;
protected:

	void ClearData();

	void SetJumpingState( ioBaseChar *pChar, ioPlayStage* pStage );

	void SetActionState( ioBaseChar *pChar, ioPlayStage* pStage, bool bSend = true );
	void SetAirActionState( ioBaseChar *pChar, ioPlayStage* pStage, bool bSend = true );

	void SetLoopState( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetAirLoopState( ioBaseChar* pChar, ioPlayStage* pStage );
	bool CheckLoopState( ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckKeyInput( ioBaseChar *pChar );
	void ProcessRotate( ioBaseChar *pChar, float fRotateSpeed );
	void CheckMotion( ioBaseChar* pChar, ioPlayStage* pStage );
	void CheckMagicCircle( ioBaseChar* pChar, ioPlayStage* pStage );

	void UpdateEffect( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar* pChar, ioPlayStage* pStage );

public:
	ioTwoStateAttack();
	ioTwoStateAttack( const ioTwoStateAttack &rhs );
	virtual ~ioTwoStateAttack();
};

inline ioTwoStateAttack* ToTwoStateAttack( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TWO_STATE_ATTACK )
		return NULL;

	return dynamic_cast< ioTwoStateAttack* >(pAttack);
}

