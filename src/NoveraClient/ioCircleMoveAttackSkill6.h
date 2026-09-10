#pragma once


#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioFullShotSkill.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMoveAttackSkill6 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PREDELAY,
		SS_WAIT,
		SS_UNDERGROUND,
		SS_WAIT_ACTION,
		SS_ACTION,
		SS_END,
	};

	enum SkillSync
	{
		SSC_ACTION,
		SSC_WAIT,
		SSC_DIR,
	};

	enum WaitType
	{
		WT_NORMAL		= 1,
		WT_IMMEDIATE	= 2,
		WT_AUTO_RANGE	= 3,
		WT_TIME_LIMIT	= 4,
	};

	enum InputType
	{
		IT_A,
		IT_S,
		IT_D,
	};

protected:
	CEncrypt<int> m_State;

	// Attack
	DWORD m_dwFireStartTime;

	int m_iMaxWeaponWay;
	int m_iWeaponWay;
	BulletInfoList	m_BulletInfoList;
	AttackAttribute m_AttackAttribute;
	ioHashString	m_stSkillMotion;
	float			m_fSkillMotionRate;
	float			m_fCircleLandOffsetHeight;
	//////////////////////////////////////////////////////////////////////////

	// Circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	// Char
	D3DXVECTOR3 m_vCharMoveDir;

	ioHashString m_ReadyCircle;
	ioHashString m_NotReadyCircle;
	ioHashString m_TargetCircleEffect;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	UniqueObjID m_dwCurMapEffect;
	UniqueObjID m_dwTargetCircleEffect;

	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	//
	WaitType m_WaitType;
	CEncrypt<DWORD> m_dwWaitLimitTime;
	DWORD m_dwWaitStartTime;

	//버프
	ioHashStringVec m_vBuffString;

	//땅속으로 들어 갈때
	DWORD			m_dwUndergroundStartTime;
	DWORD			m_dwUndergroundMaxTime;
	ioHashString	m_UndergroundEffect;
	UniqueObjID		m_dwUndergroundEffect;
	ioHashString	m_stUndergroundAni;
	float			m_fUndergroundAniRate;

	//땅속에서 나올 때
	ioHashString    m_CollisionPassBuff;
	DWORD			m_dwGroundStartTime;
	DWORD			m_dwGroundMaxTime;
	DWORD			m_dwGroundWaitTime;
	ioHashString	m_GroundEffect;
	UniqueObjID		m_dwGroundEffec;
	ioHashString	m_stGroundAni;
	float			m_fGroundAniRate;
	DWORD			m_dwGroundSpeed;
	float			m_fGround_Under_Length1;
	float			m_fGround_Under_Length2;
	WeaponInfoList  m_vWeaponInfoList;

	bool m_bEnableCharRot;
	DWORD m_dwCharRotSpeed;

	bool m_bTargetRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	bool m_bSkillInJump;

	ioHashString m_stMapEffect;
	D3DXVECTOR3  m_vEffectScale;
	UniqueObjID m_dwFlyMoveEffect;

	// 스킬 게이지 감소
	CEncrypt<bool> m_bDisableGrowth;
	CEncrypt<float> m_fGuagePerTic;
	CEncrypt<DWORD> m_dwTicTime;

	DWORD m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	float m_fSkillEndJumpPower;

	//
	float			m_fCircleToCircleOffset;
	bool			m_bOffCircle;
	ioHashString	m_CurCircleEffect;
	bool			m_bEnableSummon;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();
	virtual bool IsAttackEndState() const;

public:
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual SkillCharColType IsCharSkipState() const;
	virtual bool IsSkipState() const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	bool IsJumpSkill();
	int	 GetState();
	float GetEndJumpPower();

protected:
	void ProcessAction( ioBaseChar *pChar );
	void ProcessEnd( ioBaseChar *pChar );

	void SetPreDelayState( ioBaseChar *pChar );
	void SetWaitState( ioBaseChar *pChar );
	void SetUndergroundState( ioBaseChar *pChar );
	void SetActionWaitState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	void ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckCharDirKey( ioBaseChar *pChar );
	void CheckCharRotate( ioBaseChar *pChar );

	void EndUndergroundMapEffect( ioBaseChar *pChar );

	void AddBuffList( ioBaseChar *pChar );
	void EraseBuffList( ioBaseChar *pChar );

	void ChangeCircleDropZoneAndPositionGap( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioCircleMoveAttackSkill6();
	ioCircleMoveAttackSkill6( const ioCircleMoveAttackSkill6 &rhs );
	virtual ~ioCircleMoveAttackSkill6();
};

inline ioCircleMoveAttackSkill6* ToCircleMoveAttackSkill6( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE_ATTACK6 )
		return NULL;

	return dynamic_cast< ioCircleMoveAttackSkill6* >( pSkill );
}

