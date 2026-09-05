#pragma once


#include "ioNormalSkill.h"
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioMultiCircleMoveAttackSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_ACTION,
		SS_FIRE_WAIT,
		SS_FIRE,
		SS_END,
	};

	enum AreaWeaponState
	{
		AS_DELAY,
		AS_MOVE,
	};

	enum GaugeUseType
	{
		GUT_ON_KEY_INPUT,
		GUT_ON_SKILL_USE,
		GUT_BOTH,
	};

	enum CreateWeaponType
	{
		CWT_NONE,
		CWT_AFTER,
	};

protected:
	CEncrypt<int> m_State;

	DWORD m_dwFireStartTime;
	DWORD m_dwActionStartTime;

	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;
	ioHashString m_TargetCircleEffect;

	ioHashString m_RedGuidCircle;
	ioHashString m_BlueGuidCircle;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	UniqueObjID m_dwTargetCircleEffect;

	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	ioHashString m_LoopAnimation;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopTime;

	CEncrypt<DWORD> m_dwSkillProtectTime;
	CEncrypt<DWORD> m_dwLoopProtectDuration;
	DWORD           m_dwProtectTime;

	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	bool m_bEnableControl;
	D3DXVECTOR3 m_vMoveDir;
	float m_fMoveSpeed;
	float m_fMoveRange;

	CreateWeaponType	m_createWeaponTpye;
	bool				m_bCreatedWeapon;

	//////////////////////////////////////////////////////////////////////////
	int  m_nMaxMoveCnt;
	int  m_nCurMoveCnt;

	ioHashString m_stActoinAni;
	float		 m_fActionAniRate;

	ioHashString m_stLoopAni;
	float		 m_fLoopAniRate;

	ioHashString m_vFireDir;

	DWORD m_dwMotionEndTime;

	float m_fWarpFindLength;

	//적을 찾았고 delay시간후 쏠 준비를 하고 있다.
	bool  m_bWaitFire;

	DWORD m_dwFireDelayTime;
	DWORD m_dwFireTime;

	D3DXVECTOR3 m_vFireDestPos;
	float m_fFireWeaponOffSet;

	DWORD m_dwFireWeaponIndex;
	//////////////////////////////////////////////////////////////////////////

	// Attribute
	bool m_bUseAttack;

	CEncrypt<float> m_fAttackOffset;
	CEncrypt<float> m_fAttackHeightOffset;

	D3DXVECTOR3 m_vAttackDir;
	DWORD m_dwAttackCheckTime;


protected:
	CEncrypt<DWORD> m_dwCircleCheckDuration;
	DWORD m_dwCircleCheckTime;

	GaugeUseType m_GaugeUseType;

	CEncrypt<bool> m_bHandMeshOnDelayAni;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

	void CreateMagicCircle( ioBaseChar *pOwner );

protected:

	void SetWaitState( ioBaseChar *pChar, bool bSendNet );
	void SetFireWaitState( ioBaseChar *pOwner, bool bSendNet );
	void SetActionState( ioBaseChar *pChar, bool bSendNet );
	//void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar, bool bSendNet );

	//void CheckControl( ioBaseChar *pChar );
	//void ProcessControl( ioBaseChar *pCChar, ioPlayStage *pStage );

	bool FindEnemy(ioBaseChar *pOwenr );
	void FireWeapon( ioBaseChar *pOwner, bool bSendNet );


public:
	ioMultiCircleMoveAttackSkill();
	ioMultiCircleMoveAttackSkill( const ioMultiCircleMoveAttackSkill &rhs );
	virtual ~ioMultiCircleMoveAttackSkill();
};

inline ioMultiCircleMoveAttackSkill* ToMultiCircleMoveAttackSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_MULTI_CIRCLE_MOVE_ATTACK )
		return NULL;

	return dynamic_cast< ioMultiCircleMoveAttackSkill* >( pSkill );
}

