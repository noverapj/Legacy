#pragma once

#include "ioNormalSkill.h"
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

struct TargetDistanceSq
{
	ioBaseChar *pChar;
	float fDistanceSq;
};
typedef std::vector< TargetDistanceSq > TargetDistanceList;

class TargetDistanceSort : public std::binary_function< const TargetDistanceSq&, const TargetDistanceSq&, bool >
{
public:
	bool operator()( const TargetDistanceSq &lhs , const TargetDistanceSq &rhs ) const
	{		
		if( lhs.fDistanceSq < rhs.fDistanceSq )
		{
			return true;
		}

		return false;
	}
};

struct TargetWeaponInfo
{
	D3DXVECTOR3 m_vTargetPos;
	int m_iWeaponIndex;
	bool m_bAir;

	TargetWeaponInfo()
	{
		m_iWeaponIndex = 0;
		m_bAir = false;
	}
};
typedef std::vector< TargetWeaponInfo > TargetWeaponInfoList;


////////////////////////////////////////////////////////////////////////////////////////

class ioTargetDummyCharSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
	};

	enum SkillAttackType
	{
		SAT_DUMMY	= 1,
		SAT_WEAPON	= 2,
		SAT_ALL		= 3,
	};

	enum SkillSyncState
	{
		SSS_ACTION,
		SSS_UPDATE,
		SSS_DUMMY,
	};

protected:
	CEncrypt<int> m_State;

	DWORD m_dwFireStartTime;
	DWORD m_dwActionStartTime;

	SkillAttackType m_SkillAttackType;

// dummychar
protected:
	TargetColType m_TargetColType;
	TargetWoundType m_TargetWoundType;

	CEncrypt<int> m_iMaxTarget;

	CEncrypt<float> m_fSkillRange;
	CEncrypt<float> m_fSkillAngle;
	CEncrypt<float> m_fOffSet;
	CEncrypt<float> m_fHeightOffSet;

	CEncrypt<float> m_fUpHeight;
	CEncrypt<float> m_fUnderHeight;
	CEncrypt<float> m_fWidth;

	ioHashString m_DummyCharName;

	CEncrypt<float> m_fFrontOffSet;
	CEncrypt<float> m_fSideOffSet;

	WeaponInfo m_WeaponInfo;
	WeaponInfo m_AirWeaponInfo;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetActionState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

	void CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioTargetDummyCharSkill();
	ioTargetDummyCharSkill( const ioTargetDummyCharSkill &rhs );
	virtual ~ioTargetDummyCharSkill();
};

inline ioTargetDummyCharSkill* ToTargetDummyCharSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_DUMMY_CHAR )
		return NULL;

	return dynamic_cast< ioTargetDummyCharSkill* >( pSkill );
}

