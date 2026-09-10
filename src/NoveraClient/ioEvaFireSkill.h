#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

struct EvaAimInfo
{
	ioHashString	m_szAimMark;
	UniqueObjID		m_dwAimMarkID;
	DWORD			m_dwAimTime;
	ioHashString	m_szZeroAimMark;
	UniqueObjID		m_dwZeroAimMarkID;
	DWORD			m_dwZeroAimTime;
	WeaponInfoList	m_AimWeaponInfoList;
	WeaponInfoList	m_ZeroAimWeaponInfoList;

	EvaAimInfo()
	{
		Init();
	}

	void Init()
	{
		m_szAimMark.Clear();
		m_szZeroAimMark.Clear();
		m_AimWeaponInfoList.clear();
		m_ZeroAimWeaponInfoList.clear();

		m_dwAimMarkID		= -1;
		m_dwAimTime			= 0;
		m_dwZeroAimMarkID	= -1;
		m_dwZeroAimTime		= 0;
	}
};
typedef std::vector<EvaAimInfo> EvaAimInfoList;

struct EvaTargetInfo
{
	ioHashString	m_TargetName;
	float			m_fLenth;

	EvaTargetInfo()
	{
		m_TargetName.Clear();
		m_fLenth = 0.0f;
	}
};
typedef std::vector<EvaTargetInfo> EvaTargetInfoList;

class ioEvaFireSkill : public ioAttackSkill
{
public:
	enum SendType
	{
		SNT_CHANGE	= 1,
		SNT_ATTACK	= 2,
	};

	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ALL,
		SS_WAIT,
		SS_FIRE,
		SS_WILL_DIE,
	};

protected:
	SkillState			m_SkillState;

	ioHashString		m_SkillUpMotion;
	ioHashString		m_SkillDownMotion;

	ioHashString		m_SkillLoopMotion;
	ioHashString		m_SkillUpLoopMotion;
	ioHashString		m_SkillDownLoopMotion;

	float				m_fCurMotionRate;

	DWORD				m_dwLoopStartTime;
	DWORD				m_dwLoopDuration;

	D3DXVECTOR3			m_vAttackDir;

	DWORD				m_dwFireStartTime;

	ioHashString		m_ExtraGuidEffect;
	UniqueObjID			m_dwExtraGuidEffect;

	DWORD				m_dwFireGuidDuration;


	DWORD				m_dwAimCheckTime;
	int					m_iAimMaxCnt;
	int					m_iAimCombo;
	EvaAimInfoList		m_vEvaAimInfoList;

	// TargetList
	EvaTargetInfoList	m_vEvaTargetInfoList;
	ioHashString		m_szTargetName;
	bool				m_bJumpKey;
	bool				m_bDefenseKey;

	// Fire Effect
	ioHashString		m_szAimFireEffect;

	CEncrypt<bool>		m_bVisibleAimMarkOnlyOwner;

	//////////////////////////////////////////////////////////////////////////
	float				m_fReduceTicGauge;
	DWORD				m_dwReduceGaugeTicTime;
	DWORD				m_dwCurTic;
	DWORD				m_dwOldTime;

	bool				m_bEnableDownAim;

	DWORD				m_dwSkillProtectTime;

	bool				m_bUseWaitTime;
	DWORD				m_dwWaitTime;
	DWORD				m_dwNextFireTime;
	bool				m_bReservedFireFromWaitTime;

	bool				m_bCheckEnableSkill;	// 스킬 활성화/비활성화 체크

protected:
	TargetWoundType		m_TargetWoundType;
	float				m_fTargetAngle;
	float				m_fTargetRange;
	float				m_fTargetUpHeight;
	float				m_fTargetDownHeight;

	float				m_fCoolTimeValue;

	bool				m_bAllFireState;

protected:
	void ClearData();

	void CalcCoolTimeValue( ioBaseChar *pChar );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadWeaponInfo( const char *szWeaponInfo,
		WeaponInfoList &rkWeaponInfoList,
		ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsEnableReserve() const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

	virtual bool IsCheckEnableSkill( int nSlot, ioBaseChar *pChar );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetFireState( ioBaseChar *pOwner );

	void CheckAimTime();
	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckVDir( ioBaseChar *pOwner );
	void CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage );	

	bool IsEnableTargetState( ioBaseChar *pTarget );

	void UpdateAimMark( ioBaseChar *pChr, ioPlayStage *pStage );
	void DestroyAimMark( ioBaseChar *pChar );

	void UpdateReduceGauge( ioBaseChar *pOwner );

public:
	ioEvaFireSkill();
	ioEvaFireSkill( const ioEvaFireSkill &rhs );
	virtual ~ioEvaFireSkill();
};

inline ioEvaFireSkill* ToEvaFireSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_EVA_FIRE )
		return NULL;

	return dynamic_cast< ioEvaFireSkill* >( pAttack );
}
