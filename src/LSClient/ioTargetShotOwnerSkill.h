#pragma once

#include "ioNormalSkill.h"

class ioINILoader;
class ioBaseChar;

class ioTargetShotOwnerSkill : public ioNormalSkill
{
public:
	struct AimInfo
	{
		ioHashString	m_szAimMark;
		UniqueObjID		m_dwAimMarkID;
		DWORD			m_dwAimTime;
		ioHashString	m_szZeroAimMark;
		UniqueObjID		m_dwZeroAimMarkID;
		DWORD			m_dwZeroAimTime;
		WeaponInfo		m_AimWeaponInfo;
		WeaponInfo		m_ZeroAimWeaponInfo;

		AimInfo()
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
			m_AimWeaponInfo.Init();
			m_ZeroAimWeaponInfo.Init();
		}
	};
	typedef std::vector<AimInfo> AimInfoList;

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
		SS_FIRE_DELAY,
		SS_FIRE,
		SS_END,
	};

protected:
	SkillState m_SkillState;

	ioHashString m_SkillLoopMotion;

	DWORD m_dwLoopStartTime;
	CEncrypt<DWORD> m_dwLoopDuration;
	CEncrypt<DWORD> m_dwFireStartTime;
	CEncrypt<DWORD> m_dwFireDelayTime;

	ioHashString m_szFireSound;

	D3DXVECTOR3 m_vAttackDir;

	CEncrypt<DWORD> m_dwSkillProtectTime;

	// Aim Time
	DWORD			m_dwAimCheckTime;
	DWORD			m_dwZeroAimCheckTime;
	bool			m_bZeroAimFire;

	// Aim Mark
	int				m_iAimType;
	AimInfoList		m_vAimInfoList;

	// Aim Range
	CEncrypt<float>	m_fAimRange;

	// Aim Target Effect
	ioHashString m_szAimTargetEffect;
	ioHashString m_szZeroAimTargetEffect;

	// TargetList
	TargetInfoList	m_vTargetInfoList;
	ioHashString	m_szTargetName;
	bool			m_bJumpKey;
	bool			m_bDefenseKey;

	CEncrypt<bool>  m_bInvisibleCharMesh;

	bool			m_bSkillInJump;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();
	virtual bool IsNoDropState() const;

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

public:
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetFireDelayState( ioBaseChar *pOwner );
	void SetFireState( ioBaseChar *pOwner );
	void SetEndState( ioBaseChar *pOwner );

	void CheckAimTime();
	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckVDir( ioBaseChar *pOwner );
	void CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage );	

	void UpdateAimMark( ioBaseChar *pChr, ioPlayStage *pStage );
	void DestroyAimMark( ioBaseChar *pChar );

public:
	virtual bool IsSkipState() const;
	void SetPositionOwner( ioBaseChar *pOwner, D3DXVECTOR3 vPos );

public:
	ioTargetShotOwnerSkill();
	ioTargetShotOwnerSkill( const ioTargetShotOwnerSkill &rhs );
	virtual ~ioTargetShotOwnerSkill();
};

inline ioTargetShotOwnerSkill* ToTargetShotOwnerSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal= ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_SHOT_OWNER )
		return NULL;

	return dynamic_cast< ioTargetShotOwnerSkill* >( pNormal );
}

