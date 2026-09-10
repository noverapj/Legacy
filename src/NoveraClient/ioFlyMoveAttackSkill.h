#pragma once

#include "ioNormalSkill.h"

class ioFlyMoveAttackSkill : public ioNormalSkill
{
protected:
	enum SkillState
	{
		SS_None,
		SS_Loop,
		SS_Action,
		SS_UpDown,
		SS_End,
		SS_CreateWeapon,
		SS_Pre_Move,
		SS_Grappling,
	};

	enum SkillAttackType
	{
		SAT_Loop,
		SAT_Pre_Move,
	};

protected:
	typedef struct tagSubWeaponInfo
	{
		D3DXVECTOR3 m_vPos;
		float m_fAngle;
		WeaponInfo m_WeaponInfo;
	}SubWeaponInfo;

	typedef struct tagAreaWeaponInfo
	{
		D3DXVECTOR3 m_vPos;
		ioHashString m_szAreaWeapon;
		int m_nCreateType;
	}AreaWeaponInfo;

protected:
	DWORD m_dwSkillProtectTime;
	DWORD m_dwAttackTime;
	BuffInfoList m_OwnerBuffList;

	ioHashString m_szDelayLoopAni;
	float m_fDelayLoopAniRate;
	ioHashString m_szMoveLoopAni;
	float m_fMoveLoopAniRate;

	AttackAttribute m_AttackInfo;

	float m_fUpDownMoveSpeed;

	float m_fGaugePerTic;
	DWORD m_dwTicTime;
	float m_fExtraDecreaseGauge;
	float m_fGaugeDecreaseHeight;

	bool m_bNoInputKey;

	bool m_bCreateWeapon;
	bool m_bAttachZoneEffectWeapon;
	WeaponInfo m_AttachWeapon;
	std::vector<SubWeaponInfo> m_SubWeaponDataList;

	AreaWeaponInfo m_AreaWeaponInfo;

	bool			m_bDisableLoopMove;
	DWORD			m_dwLoopDuration;

	AttackAttribute	m_PreAttackAttribute;

	float  m_fMoveSpeed;
	float  m_fMaxMoveDist;

	float m_fFlyTargetRange;
	float m_fFlyTargetAngle;
	
	float m_fTargetRange;
	float m_fTargetAngle;

	bool m_bCheckWounedState;
	TargetWoundType m_TargetWoundType;
protected:
	SkillState m_SkillState;
	SkillAttackType m_SkillAttackType;
	DWORD m_dwCurSkillProtectTime;
	DWORD m_dwCurAttackTime;
			
	bool m_bSetUpKey;
	bool m_bSetDownKey;
	bool m_bSetDelay;
	bool m_bSetMove;

	DWORD m_dwCheckGaugeTime;
	float m_fCurExtraDecreaseGauge;

	DWORD m_dwAreaWeaponIndex;

	DWORD m_dwLoopEndTime;
	DWORD	m_dwMoveAttackWeaponIndex;
	float  m_fCurMoveDist;
	
	D3DXVECTOR3 m_vTargetDir;

	float m_fTargetGapRange;
	float m_fCharHeightRate;

	ioHashStringVec	m_vGrapplingTargetBuff;
	ioHashString	m_szTargetName;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;
	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );
protected:
	void LoadBuffList( ioINILoader &rkLoader );
	void AddOwnerBuff( ioBaseChar *pChar );
	void RemoveOwnerBuff( ioBaseChar *pChar );
	bool IsTargetWoundedState( ioBaseChar* pTarget );
protected:
	void LoadGrapplingTargetBuffList( ioINILoader &rkLoader );
	void AddGrapplingTargetBuff( ioBaseChar *pTarget, ioBaseChar *pChar );
	void RemoveGrapplingTargetBuff( ioBaseChar *pTarget, ioBaseChar *pChar );

protected:
	void Init();
	bool CheckKeyInput( ioBaseChar *pChar );
	bool CheckAttackInput( ioBaseChar *pChar );
	void CheckUpDownMoveInput( ioBaseChar *pChar );

protected:
	void CheckDelayMoveAni( ioBaseChar *pOwner );
	void ProcessUpDownMove( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void SetLoopState( ioBaseChar *pChar );
	void SetAttackState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

protected:
	void CreateWeapon( ioBaseChar *pChar );

protected:
	void SetPreMoveState( ioBaseChar *pChar );
	ioHashString FindAttTarget( ioBaseChar *pOwner, float fAngle, float fRange );
	bool CheckMove( ioBaseChar *pChar, ioPlayStage *pStage );
public:
	ioFlyMoveAttackSkill();
	ioFlyMoveAttackSkill( const ioFlyMoveAttackSkill &rhs );
	virtual ~ioFlyMoveAttackSkill();
};

inline ioFlyMoveAttackSkill* ToFlyMoveAttackSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_FLY_MOVE_ATTACK )
		return NULL;

	return dynamic_cast< ioFlyMoveAttackSkill* >( pSkill );
}
