#pragma once

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioChargingFireSkill4 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_FAIL,
	};

	enum LoopAimState
	{
		LAS_NONE,
		LAS_WAIT,
		LAS_AIMED,
	};

	enum SkillSync
	{
		SSC_LOOP,
		SSC_ACTION,
		SSC_DIR,
		SSC_TARGET,
	};

protected:
	CEncrypt<int> m_State;
	LoopAimState m_LoopAimState;

	// Attack
	AttackAttribute m_AttackAttribute;

	ioHashString m_SkillLoopMotion;
	float m_fSkillLoopMotionRate;

	DWORD m_dwMaxChargingTime;
	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;

	// fail
	ioHashString m_SkillFailMotion;
	float m_fSkillFailMotionRate;

	// target
	CEncrypt<bool> m_bEnableDownAim;

	float m_fTargetAimRate;
	float m_fDownTargetAimRate;

	ioHashString m_szTargetName;

	DWORD m_dwAimStartTime;
	CEncrypt<DWORD> m_dwAimTime;

	CEncrypt<float> m_fAimMinRange;
	CEncrypt<float> m_fAimMaxRange;
	CEncrypt<float> m_fAimAngle;
	CEncrypt<float> m_fMarkerRange;

	ioHashString m_szReadyMarker;
	ioHashString m_szAimingMarker;
	ioHashString m_szAimedMarker;
	ioHashString m_szZeroAimedMarker;

	bool m_bSetAimedTarget;
	D3DXVECTOR3 m_vAimedPos;

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
	void SetFailState( ioBaseChar *pChar );

	void ProcessLoopState(  ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessAimState(  ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckCharRotateKey( ioBaseChar *pChar );

	void SetAimState( ioBaseChar *pChar, bool bSend );
	void CheckAimedState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetAimedState( ioBaseChar *pChar, const ioHashString &szTarget );

	void TarckingTarget( ioBaseChar *pChar );
	bool CheckTargetValidate( ioBaseChar *pChar );

	D3DXVECTOR3 ReCalcMoveDir( ioBaseChar *pChar, ioWeapon *pWeapon );

	bool CheckDownAimState( ioBaseChar *pTarget );

public:
	ioChargingFireSkill4();
	ioChargingFireSkill4( const ioChargingFireSkill4 &rhs );
	virtual ~ioChargingFireSkill4();
};

inline ioChargingFireSkill4* ToChargingFireSkill4( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CHARGING_FIRE4 )
		return NULL;

	return dynamic_cast< ioChargingFireSkill4* >( pSkill );
}

