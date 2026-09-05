#pragma once
#include "ioAttackSkill.h"
class ioTargetTeleportAttackSkill :	public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_AIM,
		SS_TELEPORT_ATTACK,
		SS_LAND_ATTACK,
		SS_END,
	};

	enum SkillSyncType
	{
		SST_TARGET,
		SST_TELEPORT_ATTACK,
	};
protected:
	SkillState m_SkillState;

	//Aim
	ioHashString			m_szAimAni;
	float					m_fAimRange;
	bool					m_bEnableDownAim;
	ioHashString			m_szAimedMarker;

	BaseCharDistanceSqList	m_vTargetInfoList;
	ioHashString			m_szAimedTarget;
	D3DXVECTOR3				m_vAimDir;
	bool					m_bChangeTargetPrev;
	bool					m_bChangeTargetNext;
	DWORD					m_dwMakerID;

	//Aim time
	DWORD					m_dwLoopTime;

	//Key Check
	DWORD					m_dwAttackDelayTime;
	bool					m_bInputAttackKey;
	DWORD					m_dwAttackKeyInputTime;

	//Attack Offset
	D3DXVECTOR3 m_vNoTargetTeleportOffset;
	D3DXVECTOR3 m_vTargetTeleportOffset;

	//Land Attack
	AttackAttribute m_LandAttackAttribute;

	//End Buff
	ioHashStringVec m_vOwnerEndBuffList;
	bool			m_bUseEndBuff;
protected:
	void ClearData();
	void SetAimState( ioBaseChar *pChar );
	void SetTeleport( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetAttackState( ioBaseChar *pChar );
	void SetLandAttackState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

protected:
	void SetTargetRotation( ioBaseChar *pChar );
	void GetTargetList( ioBaseChar *pChar, ioPlayStage *pStage );

	bool CheckInputChangeTarget( ioBaseChar *pChar );
	bool CheckInputAttack( ioBaseChar *pChar );
	bool CheckLanding( ioBaseChar *pChar );
	
protected:
	void UpdateMaker( ioBaseChar *pChar, ioPlayStage *pStage );
	DWORD CreateEffect( ioBaseChar *pChar, ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale );
	void DestroyEffect( ioBaseChar *pChar, ioPlayStage *pStage, DWORD& dwEffectID );
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual void SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon = NULL );

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool IsEndState() const;
	virtual bool IsNoDropState() const;

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
public:
	virtual bool CheckExceptionCamera();
	virtual D3DXVECTOR3 GetExceptionCameraPos( ioBaseChar *pChar );
public:
	ioTargetTeleportAttackSkill(void);
	ioTargetTeleportAttackSkill( const ioTargetTeleportAttackSkill &rhs );
	virtual ~ioTargetTeleportAttackSkill(void);
};


inline ioTargetTeleportAttackSkill* ToTargetTeleportAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TARGET_TELEPORT_ATTACK )
		return NULL;

	return dynamic_cast< ioTargetTeleportAttackSkill* >( pAttack );
}