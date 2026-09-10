#pragma once

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioChargingFireSkill2 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
	};

	enum SkillSync
	{
		SSC_LOOP,
		SSC_ACTION,
		SSC_DIR,
	};

protected:
	CEncrypt<int> m_State;

	// Attack
	AttackAttribute m_AttackAttribute;

	ioHashString m_SkillLoopMotion;
	float m_fSkillLoopMotionRate;

	DWORD m_dwMaxChargingTime;
	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;

	// Char
	D3DXVECTOR3 m_vCharMoveDir;

	DWORD m_dwLoopStartTime;
	DWORD m_dwAttackEndTime;
	DWORD m_dwLoopDuration;

	ioHashString m_SkillChargingEffect;
	UniqueObjID m_dwChargingEffect;

	//
	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	//
	bool m_bTargetRot;
	DWORD m_dwCharRotateSpeed;

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

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend );

	void ProcessLoopState(  ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckCharRotateKey( ioBaseChar *pChar );
	void ProcessCharRotateDir( ioBaseChar *pChar );

public:
	ioChargingFireSkill2();
	ioChargingFireSkill2( const ioChargingFireSkill2 &rhs );
	virtual ~ioChargingFireSkill2();
};

inline ioChargingFireSkill2* ToChargingFireSkill2( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CHARGING_FIRE2 )
		return NULL;

	return dynamic_cast< ioChargingFireSkill2* >( pSkill );
}

