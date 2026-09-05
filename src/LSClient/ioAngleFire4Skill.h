#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"


struct AimInfo2
{
	ioHashString	m_szAimMark;
	UniqueObjID		m_dwAimMarkID;
	DWORD			m_dwAimTime;
	ioHashString	m_szZeroAimMark;
	UniqueObjID		m_dwZeroAimMarkID;
	DWORD			m_dwZeroAimTime;
	WeaponInfoList	m_AimWeaponInfoList;
	WeaponInfoList	m_ZeroAimWeaponInfoList;

	AimInfo2()
	{
		Init();
	}

	void Init()
	{
		m_szAimMark.Clear();
		m_dwAimMarkID	= -1;
		m_dwAimTime		= 0;
		m_szZeroAimMark.Clear();
		m_dwZeroAimMarkID	= -1;
		m_dwZeroAimTime		= 0;
		m_AimWeaponInfoList.clear();
		m_ZeroAimWeaponInfoList.clear();
	}
};
typedef std::vector<AimInfo2> AimInfoList2;

struct TargetInfo2
{
	ioHashString	m_TargetName;
	float			m_fLenth;

	TargetInfo2()
	{
		m_TargetName.Clear();
		m_fLenth = 0.0f;
	}
};
typedef std::vector<TargetInfo2> TargetInfoList2;

class ioAngleFire4Skill : public ioAttackSkill
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
		SS_WAIT,
		SS_FIRE,
		SS_WILL_DIE,
	};

protected:
	SkillState m_SkillState;

	ioHashString m_SkillUpMotion;
	ioHashString m_SkillDownMotion;

	ioHashString m_SkillLoopMotion;
	ioHashString m_SkillUpLoopMotion;
	ioHashString m_SkillDownLoopMotion;

	float m_fCurMotionRate;

	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;

	D3DXVECTOR3 m_vAttackDir;

	DWORD m_dwFireStartTime;

	ioHashString m_ExtraGuidEffect;
	UniqueObjID m_dwExtraGuidEffect;

	DWORD m_dwFireGuidDuration;

	// Limit Angle
	//CEncrypt<float> m_fMaxAngle;
	//CEncrypt<float> m_fMinAngle;
	//CEncrypt<float>	m_fAimRange;

	// Aim Time
	DWORD			m_dwAimCheckTime;
	//DWORD			m_dwZeroAimCheckTime;
	//bool			m_bZeroAimFire;

	// Aim Mark
	int				m_iAimType;
	AimInfoList2		m_vAimInfoList;

	// TargetList
	TargetInfoList2	m_vTargetInfoList;
	ioHashString	m_szTargetName;
	bool			m_bJumpKey;
	bool			m_bDefenseKey;

	// Fire Effect
	ioHashString m_szAimFireEffect;
	//ioHashString m_szZeroAimFireEffect;

	CEncrypt<bool> m_bVisibleAimMarkOnlyOwner;

	//////////////////////////////////////////////////////////////////////////
	float m_fReduceTicGauge;
	DWORD m_dwReduceGaugeTicTime;
	DWORD m_dwCurTic;
	DWORD m_dwOldTime;

	bool  m_bEnableDownAim;

	DWORD m_dwSkillProtectTime;

	bool m_bUseWaitTime;
	DWORD	m_dwWaitTime;
	DWORD m_dwNextFireTime;
	bool m_bReservedFireFromWaitTime;

//
protected:
	TargetWoundType m_TargetWoundType;
	CEncrypt<float> m_fTargetAngle;
	CEncrypt<float> m_fTargetRange;
	CEncrypt<float> m_fTargetUpHeight;
	CEncrypt<float> m_fTargetDownHeight;

	float			m_fCoolTimeValue;

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
	ioAngleFire4Skill();
	ioAngleFire4Skill( const ioAngleFire4Skill &rhs );
	virtual ~ioAngleFire4Skill();
};

inline ioAngleFire4Skill* ToAngleFire4Skill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_ANGLE_FIRE4 )
		return NULL;

	return dynamic_cast< ioAngleFire4Skill* >( pAttack );
}

/*class ChangeLenthSort2 : std::binary_function< const TargetInfo2&, const TargetInfo2&, bool >
{
public:
	bool operator()( const TargetInfo2 &lhs , const TargetInfo2 &rhs ) const
	{
		if( lhs.m_fLenth < rhs.m_fLenth )
		{
			return true;
		}
		return false;
	}
};*/

