#pragma once

#include "ioAttackSkill.h"

class ioLaserControlSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_ACTION_LOOP,
		SS_END,
	};

	enum SkillSync
	{
		SSC_ACTION,
		SSC_END,
	};

protected:
	CEncrypt<int> m_State;

	AttackAttribute m_AttackAttribute;
	DWORD m_dwProtectTime;
	DWORD m_dwCurProtectTime;

	ioHashString m_SkillDownMotion;
	ioHashString m_SkillUpMotion;

	ioHashString m_SkillActionLoopMotion;
	ioHashString m_SkillActionLoopUpMotion;
	ioHashString m_SkillActionLoopDownMotion;

	ioHashString m_SkillLoopMotion;
	ioHashString m_SkillLoopUpMotion;
	ioHashString m_SkillLoopDownMotion;

	D3DXVECTOR3 m_vAttackDir;

	float m_fSkillLoopMotionRate;
	float m_fSkillActionLoopMotionRate;
	float m_fCurMotionRate;

	// Char
	float m_fCharJumpPower;
		
	// Control Weapon
	DWORD m_dwCurWeaponIdx;

	//loop
	DWORD m_dwLoopDuration;
	DWORD m_dwLoopDurationEnd;

	DWORD m_dwActionLoopDuration;
	DWORD m_dwActionLoopDurationEnd;

	DWORD m_dwFireStartTime;

	// Circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;	
	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
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
	void ClearData();
	void SetActionState( ioBaseChar *pChar, bool bSend );
	void SetLoopState( ioBaseChar *pChar );
	void SetActionLoopState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar* pChar, bool bSend );

	void CheckMotion( ioBaseChar* pChar );
	void CheckLoopState( ioBaseChar *pChar );
	void CheckMagicCircle( ioBaseChar *pChar );
	void CheckWeapon( ioBaseChar *pChar );

	void RemoveWeapon( ioBaseChar *pChar );

public:
	ioLaserControlSkill();
	ioLaserControlSkill( const ioLaserControlSkill &rhs );
	virtual ~ioLaserControlSkill();
};

inline ioLaserControlSkill* ToLaserControlSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_LASER_CONTROL )
		return NULL;

	return dynamic_cast< ioLaserControlSkill* >( pSkill );
}

