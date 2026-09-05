#pragma once


#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioFullShotSkill.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMoveAttackSkill4 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_ACTION,
		SS_END,
	};

	enum SkillSync
	{
		SSC_ACTION,
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
	AttackAttribute m_AttackAttribute_A;
	ioHashString	m_stSkillMotion_A;
	float			m_fSkillMotionRate_A;
	float			m_fCircleLandOffsetHeight;
	//////////////////////////////////////////////////////////////////////////
	AttackAttribute m_AttackAttribute_S;
	ioHashString	m_stSkillMotion_S;
	float			m_fSkillMotionRate_S;
	//////////////////////////////////////////////////////////////////////////
	//AttackAttribute m_AttackAttribute_D;
	ioHashString	m_stSkillMotion_D;
	float			m_fSkillMotionRate_D;
	vDummyCharLoadInfoList	m_DummyCharList;
	ioHashStringVec m_vDestroyDummyCharList;
	vDummyCharCreateInfoList m_vDummyCreateList;
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

public:
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual SkillCharColType IsCharSkipState() const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar, InputType inputType, ioPlayStage *pStage );

public:
	bool IsJumpSkill();
	int	 GetState();
	float GetEndJumpPower();

protected:
	void ProcessAction( ioBaseChar *pChar );
	void ProcessEnd( ioBaseChar *pChar );

	void SetWaitState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, InputType inputType );
	void SetEndState( ioBaseChar *pChar );

	void CheckCharDirKey( ioBaseChar *pChar );
	void CheckCharRotate( ioBaseChar *pChar );

public:
	ioCircleMoveAttackSkill4();
	ioCircleMoveAttackSkill4( const ioCircleMoveAttackSkill4 &rhs );
	virtual ~ioCircleMoveAttackSkill4();
};

inline ioCircleMoveAttackSkill4* ToCircleMoveAttackSkill4( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE_ATTACK4 )
		return NULL;

	return dynamic_cast< ioCircleMoveAttackSkill4* >( pSkill );
}

