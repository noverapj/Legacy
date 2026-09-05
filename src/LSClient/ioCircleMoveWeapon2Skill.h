#pragma once


#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMoveWeapon2Skill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_LOOP,
		SS_FIRE_ACTION,
		SS_END,
	};

	enum FireType
	{
		FT_AUTO,			//자동 공격
		FT_ATTACK_KEY,		//Loop Ani에서 Attack Key 입력시 weapon 발사
		FT_FIRE_ANI,		//Loop Ani에서 Attack Key 입력시 Fire Ani
	};

	enum
	{
		SST_RUN,
		SST_ROTATE,
		SST_CREAT_DUMMY,
		SST_ATTACK_KEY_INPUT,
		SST_FIRE_ACTION,
	};

protected:
	CEncrypt<int> m_State;

	// Attack
	DWORD	m_dwFireStartTime;
	DWORD	m_dwNextFireStartTime;
	int		m_iCurFireCount;
	
	CEncrypt<DWORD> m_dwFireGapTime;
	CEncrypt<float>	m_fFireHightOffset;
	CEncrypt<float>	m_fFireFrontOffset;
	CEncrypt<int>	m_iRandomRange;
	CEncrypt<int>	m_iTotalFireCount;

	AttackAttribute m_AttackAttribute;
	AttackAttribute m_EndAttackAttribute;
	AttackAttribute m_FireAttribute;

	// Loop Motion
	ioHashString	m_szSkillLoopMotion;
	CEncrypt<float>	m_fSkillLoopAniRate;
	CEncrypt<DWORD> m_dwSkillLoopDuration;
	CEncrypt<DWORD> m_dwSkillLoopProtectTime;
	DWORD			m_dwSkillLoopStartTime;

	// Circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;
	float m_fRevisionRate;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;
	ioHashString m_TargetCircleEffect;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	UniqueObjID m_dwTargetCircleEffect;

	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	CEncrypt<DWORD>	m_dwRotateTime;
	bool m_bTargetRot;
	bool m_bLeftRot;
	
	ioUserKeyInput::DirKeyInput	m_CurDirKey;

	// Weapon
	int m_iCurWeaponCnt;
	CEncrypt<float> m_fWeaponGravityRate;
	CEncrypt<float> m_fWeaponFloatPower;
	CEncrypt<float> m_fWeaponFloatMinPower;
	CEncrypt<float> m_fWeaponMinSpeed;
	CEncrypt<float> m_fWeaponGapDistance;

	WeaponMoveInfo m_vWeaponMoveInfo;

	float m_fUpLimitHeight;
	float m_fUpLimitRange;

	float m_fFirstTargetDistance;
	float m_fCircleSlowDownSpeedRate;

	// 스킬 게이지 감소
	CEncrypt<bool> m_bDisableGrowth;
	CEncrypt<float> m_fGuagePerTic;
	CEncrypt<DWORD> m_dwTicTime;

	DWORD m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	bool m_bSetNoMoveCircle;
	float m_fCircleMaxMoveRange;
	bool m_bSetNoInputKey;


	FireType m_FireType;
	DWORD m_dwAttackKeyInputTime;

	//Dummy
	vDummyCharLoadInfoList m_DummyCharList;
	vDummyCharCreateInfoList m_CreateDummyList;
	ioHashStringVec m_vDestroyDummyCharList;
	bool			m_bDummyLookAtCircle;

protected:
	void ClearData();

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
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;	

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );
	void SetFireState( ioBaseChar *pChar );
	void SetFireDummy( ioBaseChar *pChar );

	void OnLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessRotate( ioBaseChar *pChar );
	void CheckKeyInput( ioBaseChar *pOwner );

	void AttackFire( ioBaseChar *pChar );
	void AttackFireByKeyInput( ioBaseChar *pChar );

	void CreateWeapon( ioBaseChar *pChar );

protected:
	float GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight );
	float GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower );

	void CheckWeaponFloatInfo( ioBaseChar *pOwner );

	//Dummy
	void CreateDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessDummyRotate( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	void GenerateEndAttack( const D3DXVECTOR3 &vPos, ioBaseChar *pOwner );

public:
	ioCircleMoveWeapon2Skill();
	ioCircleMoveWeapon2Skill( const ioCircleMoveWeapon2Skill &rhs );
	virtual ~ioCircleMoveWeapon2Skill();
};

inline ioCircleMoveWeapon2Skill* ToCircleMoveWeapon2Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE_WEAPON2 )
		return NULL;

	return dynamic_cast< ioCircleMoveWeapon2Skill* >( pSkill );
}

