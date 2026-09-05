#pragma once

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioDashMoveChargeAttack : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_AIM,
		SS_TARGET,
		SS_DASH,
		SS_ATTACK,
		SS_RESERVE_ATTACK,
		SS_CREATE_SUB_WEAPON_EFFECT,
		SS_END,
	};
protected:
	typedef struct tagSubWeaponInfo
	{
		D3DXVECTOR3 m_vPos;
		WeaponInfo m_WeaponInfo;
	}SubWeaponInfo;

protected:
	float m_fFirstUseGauge;
	ioHashString m_szAimUpAni;
	ioHashString m_szAimDownAni;
	ioHashString m_szAimCenterAni;

	ioHashString m_szAimedMarker;
	float m_fAimRange;

	AttackAttribute m_DashInfo;
	AttackAttribute m_AttackInfo;
	int m_nMaxAttackCnt;

	ioHashString m_szFireUpAni;
	ioHashString m_szFireDownAni;
	
	DWORD m_dwCheckSubWeaponTime;
	ioHashString m_szSubWeaponEffect;
	std::vector<SubWeaponInfo> m_SubWeaponDataList;

	bool m_bEnableDownAim;

protected:
	//스킬 상태
	SkillState m_SkillState;
	//스킬 보호 시간
	DWORD m_dwSkillProtectTime;
	//현재 스킬 보호 시간
	DWORD m_dwCurSkillProtectTime;

	ioHashString m_szAimedTarget;
	DWORD m_dwMotionEndTime;
	DWORD m_dwSubWeaponFireTime;

	DWORD m_dwMakerID;
	D3DXVECTOR3 m_vAimDir;

	BaseCharDistanceSqList m_vTargetInfoList;

	bool m_bChangeTarget;
	bool m_bSetReserveAttack;
	bool m_bSetSubWeapon;

	bool m_bUseActiveCnt;
	int m_nAttackCnt;

	DWORD m_dwStartSubWeaponTime;
	int m_nCurSubWeaponCnt;
	std::vector<DWORD> m_SubWeaponEffectList;
	
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
	virtual bool IsEnableDashCheck( ioBaseChar *pChar );
	
protected:
	void InitData();

protected:
	DWORD CreateEffect( ioBaseChar *pChar, ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale );
	void DestroyEffect( ioBaseChar *pChar, ioPlayStage *pStage, DWORD& dwEffectID );

protected:
	void GetTargetList( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetTargetRotation( ioBaseChar *pChar );
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );
	void UpdateMaker( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void SetAimState( ioBaseChar *pChar );
	void SetDashState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetAttackState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );
	void SetAttackAni( ioEntityGroup *pGrp, float fTimeRate, DWORD dwPreDelay );

protected:
	bool CheckInputDash( ioBaseChar *pChar );
	bool CheckInputChangeTarget( ioBaseChar *pChar );
	bool CheckInputAttack( ioBaseChar *pChar );
	bool CheckReserveAttack( ioBaseChar *pChar );

protected:
	void CreateSubWeaponEffect( ioBaseChar *pChar, ioPlayStage *pStage );
	void DestroySubWeaponEffect( ioBaseChar *pChar, ioPlayStage *pStage );
	void UpdateSubWeaponEffect( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void CreateWeapon( ioBaseChar *pChar, ioPlayStage *pStage );
	void CreateSubWeapon( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	virtual bool IsUseActiveCnt() const;
	virtual int GetMaxActiveCnt( ioBaseChar *pChar ) const;
	virtual int GetCurActiveCnt( ioBaseChar *pChar ) const;

public:
	ioDashMoveChargeAttack();
	ioDashMoveChargeAttack( const ioDashMoveChargeAttack &rhs );
	virtual ~ioDashMoveChargeAttack();
};

inline ioDashMoveChargeAttack* ToDashMoveChargeAttack( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_DASH_MOVE_CHARGE_ATTACK )
		return NULL;

	return dynamic_cast< ioDashMoveChargeAttack* >( pAttack );
}