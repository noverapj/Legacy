#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"
#include "ioAngleFire2Skill.h"

class ioAngleFire5Skill : public ioAttackSkill
{
public:
	struct TargetInfo
	{
		ioHashString	m_TargetName;
		float			m_fLenth;

		TargetInfo()
		{
			m_TargetName.Clear();
			m_fLenth = 0.0f;
		}
	};
	typedef std::vector<TargetInfo> TargetInfoList;

	class ChangeLenthSort : std::binary_function< const TargetInfo&, const TargetInfo&, bool >
	{
	public:
		bool operator()( const TargetInfo &lhs , const TargetInfo &rhs ) const
		{
			if( lhs.m_fLenth < rhs.m_fLenth )
			{
				return true;
			}
			return false;
		}
	};

	enum SendType
	{
		SNT_CHANGE	= 1,
		SNT_ATTACK	= 2,
		SNT_WARP = 3,
		SNT_WARP_END = 4,
		SNT_WARP_ADD = 5,
	};

	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_FIRE,
		SS_WARP,
		SS_WARP_END,
		SS_END,
	};

protected:
	SkillState m_SkillState;

	ioHashString m_SkillUpMotion;
	ioHashString m_SkillDownMotion;
	ioHashString m_JumpSkillUpMotion;
	ioHashString m_JumpSkillDownMotion;

	ioHashString m_SkillLoopMotion;
	ioHashString m_SkillUpLoopMotion;
	ioHashString m_SkillDownLoopMotion;
	ioHashString m_JumpSkillLoopMotion;
	ioHashString m_JumpSkillUpLoopMotion;
	ioHashString m_JumpSkillDownLoopMotion;

	float m_fCurMotionRate;

	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;

	D3DXVECTOR3 m_vAttackDir;
	D3DXVECTOR3 m_vAttackPos;
	D3DXVECTOR3 m_vTargetPos;
	bool m_bTargetJump;

	DWORD m_dwFireStartTime;

	ioHashString m_ExtraGuidEffect;
	UniqueObjID m_dwExtraGuidEffect;

	DWORD m_dwFireGuidDuration;

	// Limit Angle
	CEncrypt<float> m_fMaxAngle;
	CEncrypt<float> m_fMinAngle;
	CEncrypt<float>	m_fAimRange;

	// Aim Time
	DWORD			m_dwAimCheckTime;
	DWORD			m_dwZeroAimCheckTime;
	bool			m_bZeroAimFire;

	// Aim Mark
	int				m_iAimType;
	AimInfoList		m_vAimInfoList;

	// TargetList
	TargetInfoList	m_vTargetInfoList;
	ioHashString	m_szTargetName;
	bool			m_bJumpKey;
	bool			m_bDefenseKey;

	// Fire Effect
	ioHashString m_szAimFireEffect;
	ioHashString m_szZeroAimFireEffect;
	ioHashStringVec m_szFireBuffList;

	CEncrypt<bool> m_bVisibleAimMarkOnlyOwner;

	ioHashString m_szWarpStartMotion;
	ioHashString m_szWarpJumpStartMotion;
	ioHashString m_szWarpEndMotion;

	float m_fWarpStartMotionRate;
	float m_fWarpJumpStartMotionRate;
	float m_fWarpEndMotionRate;
	float m_fJumpHeightGap;
	float m_fEndJumpValue;

	ioHashStringVec m_szWarpTargetList;

protected:
	void ClearData();

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
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock );

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetFireState( ioBaseChar *pOwner );
	void SetWarpState( ioBaseChar *pOwner, ioHashString szTargetName );
	void SetWarpEndState( ioBaseChar *pOwner );
	void SetWarpEndState( ioBaseChar *pChar, SP2Packet &rkPacket );

	void CheckAimTime();
	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckVDir( ioBaseChar *pOwner );
	void CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage );	

	void UpdateAimMark( ioBaseChar *pChr, ioPlayStage *pStage );
	void DestroyAimMark( ioBaseChar *pChar );

	void SetWarpAnimation( ioBaseChar *pOwner, ioHashString szMotion, float fMotionRate );
	
public:
	ioAngleFire5Skill();
	ioAngleFire5Skill( const ioAngleFire5Skill &rhs );
	virtual ~ioAngleFire5Skill();
};

inline ioAngleFire5Skill* ToAngleFire5Skill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_ANGLE_FIRE5 )
		return NULL;

	return dynamic_cast< ioAngleFire5Skill* >( pAttack );
}


