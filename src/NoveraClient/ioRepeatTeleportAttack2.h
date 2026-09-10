#pragma once

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioRepeatTeleportAttack2 : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ATTACK,
		SS_END,
		SS_TELEPORT,
		SS_TELEPORT_FIND_ENEMY,
	};

protected:
	float m_fSkillRange;
	float m_fSkillAngle;
	float m_fOffSet;

	float m_fBackRange;
	float m_fHeightRange;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;
	DWORD m_dwWeaponRepeatTime;
	DWORD m_dwTeleportTime;
	DWORD m_dwWeaponFireTime;
	DWORD m_dwUseGaugeTime;
	ioUserKeyInput::DirKeyInput m_CurDir;

protected:
	DWORD m_dwSkillProtectTime;
	AttackAttribute m_AttackInfo;
	ioHashString m_szLoopAni;
	DWORD m_dwCheckWeaponRepeatTime;
	DWORD m_dwCheckTeleportTime;
	DWORD m_dwCheckWeaponFireTime;
	D3DXVECTOR3 m_vTeleportOffset;

	float m_fGaugePerTic;
	DWORD m_dwTicTime;
	bool m_bNoDropState;

	ioHashString m_szTeleportStartEffect;
	ioHashString m_szTeleportEndEffect;

	DWORD m_dwDropDelay;

	DWORD m_dwReservKeyTime;
	DWORD m_dwNextReservKeyTime;

	float m_fNoneTargetMoveRange;
	ioHashString m_stNoneTargetMarkBuff;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual bool IsAttackEndState() const;

	virtual bool IsNoDropState() const;
	virtual bool IsCanSendControl() const;

protected:
	bool CheckSkillGauge( ioBaseChar *pChar );
	bool CheckKeyInput( ioBaseChar *pChar, ioPlayStage *pStage );
	bool CheckTeleport( ioBaseChar *pChar );
	bool CheckAttack( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pChar );
	void SetTelePortState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetAttackState( ioBaseChar *pChar, bool bSendPacket );
	void SetEndState( ioBaseChar *pChar );

protected:
	void TeleportMove( ioBaseChar *pChar, ioPlayStage *pStage );
	void TeleportFindEnemy( ioBaseChar *pChar, ioPlayStage *pStage, bool bSendNet = true );
	void MoveTeleport( ioBaseChar *pOwner, ioBaseChar *pTarget, ioPlayStage *pStage );

public:
	ioRepeatTeleportAttack2();
	ioRepeatTeleportAttack2( const ioRepeatTeleportAttack2 &rhs );
	virtual ~ioRepeatTeleportAttack2();
};

inline ioRepeatTeleportAttack2* ToRepeatTeleportAttack2( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_REPEAT_TELEPORT_ATTACK2 )
		return NULL;

	return dynamic_cast< ioRepeatTeleportAttack2* >( pAttack );
}

