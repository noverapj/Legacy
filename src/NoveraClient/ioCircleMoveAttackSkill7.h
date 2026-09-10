#pragma once


#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioCircleMoveAttackSkill7 : public ioNormalSkill
{
protected:
	struct RangeInfo
	{
		float m_fMaxDist;
		float m_fMinDist;
		AttackAttribute m_AttackAttribute;
		RangeInfo()
		{
			m_fMaxDist = 0.0f;
			m_fMinDist = 0.0f;
			m_AttackAttribute.Init();
		}
		bool CheckRange( float fDist )
		{
			return COMPARE( fDist, m_fMinDist, m_fMaxDist ) ? true : false;
		}
	};
	typedef std::vector<RangeInfo> vRangeInfo;

public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_ACTION,
		SS_PRE_LOOP,
		SS_LOOP,
		SS_END,
	};

	enum SkillSync
	{
		SSC_POSITION,
		SSC_ATTACK, 
	};

protected:
	CEncrypt<int> m_State;

	// Attack
	DWORD m_dwFireStartTime;

	ioHashString    m_LoopAnimation;
	CEncrypt<float> m_fLoopAniRate;
	CEncrypt<DWORD> m_dwLoopDuration;
	DWORD           m_dwLoopStartTime;

	// Circle
	CEncrypt<float> m_fCircleRange;
	CEncrypt<float> m_fCircleMoveSpeed;
	CEncrypt<float> m_fCircleHeightGap;
	CEncrypt<float> m_fCircleOffSet;
	ioHashString    m_ReadyCircle;
	ioHashString    m_RedEffect;
	ioHashString    m_BlueEffect;
	UniqueObjID     m_dwCurMapEffect;

	D3DXVECTOR3 m_vCirclePos;
	
	CEncrypt<float> m_fRangeSpeed;

	CEncrypt<float> m_fMaxRange;
	CEncrypt<float> m_fMinRange;
	float           m_fCurRange;

	vRangeInfo      m_vRangeInfo;

	ioHashString    m_SkillEffect1;
	DWORD           m_dwSkillEffect1ID;
	ioHashString    m_SkillEffect2;
	DWORD           m_dwSkillEffect2ID;
	ioHashString    m_TestEffect;
	DWORD           m_dwTestEffectID[4];

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<DWORD> m_dwSkillProtectTime;

protected:
	void ClearData();

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
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetPreLoopState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetAttackState( ioBaseChar *pChar );

	void ProcessPreLoopState( ioBaseChar *pChar );
	void ProcessLoopState( ioBaseChar *pChar );

	void RotateToNearChar( ioBaseChar *pChar );

public:
	ioUserKeyInput::DirKeyInput GetCurKey();

public:
	ioCircleMoveAttackSkill7();
	ioCircleMoveAttackSkill7( const ioCircleMoveAttackSkill7 &rhs );
	virtual ~ioCircleMoveAttackSkill7();
};

inline ioCircleMoveAttackSkill7* ToCircleMoveAttackSkill7( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE_ATTACK7 )
		return NULL;

	return dynamic_cast< ioCircleMoveAttackSkill7* >( pSkill );
}