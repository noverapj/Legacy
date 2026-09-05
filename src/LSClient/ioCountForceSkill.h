#pragma once

#include "ioCountSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioCountForceSkill : public ioCountSkill
{
protected:
	struct GlideInfo
	{
		ioHashString    m_Animation;			// 글라이드 모션.
		ioHashString    m_DelayAnimation;		// 글라이드 중 공격 모션.
		CEncrypt<DWORD> m_dwDelayDuration;	// 최대 차징 시간.
		CEncrypt<float> m_fAniRate;				// 글라이드 모션의 aniRate.
		CEncrypt<float> m_fDelayAniRate;		// 글라이드 중 공격 모션의 aniRate.
		CEncrypt<DWORD> m_dwChargingDuration;	// 최대 차징 시간.

		GlideInfo()
		{
			m_Animation.Clear();
			m_DelayAnimation.Clear();
			m_fAniRate = FLOAT1;
			m_fDelayAniRate = FLOAT1;
			m_dwChargingDuration = 0;
		}
	};

	enum SkillState
	{
		SS_NONE,
		SS_GLIDE_LOOP,
		SS_GLIDE_MOVING,
		SS_GLIDE_END,
	};

	enum SkillSync
	{
		SC_MOVING,
		SC_END,
	};

protected:
	SkillState m_SkillState;

	GlideInfo m_NormalGlideInfo;
	GlideInfo m_JumpGlideInfo;

	ioHashString    m_EndAnimation;
	CEncrypt<float> m_fEndAniRate;

	float m_fMoveRange;

	bool m_bTargetRot;
	bool m_bLeftRot;

	ioHashString m_MoveEffect;
	UniqueObjID m_dwMoveEffectID;

	float m_fLand_Forward_Force;
	float m_fLand_Backward_Force;
	float m_fJumpForward_Force;
	float m_fJumpBackward_Force;

	float m_fLand_Forward_Friction;
	float m_fLand_Backward_Friction;
	float m_fJumpForward_Friction;
	float m_fJumpBackward_Friction;

	float m_fMoveEffectPosRange;

	ioHashString m_stMoveAni;
	float m_fMoveAniRate;

	D3DXVECTOR3 m_vMoveDir;
	bool m_bLandSkill;

	DWORD m_dwProtectedLoopTime;

protected:
	void ClearData();
	void LoadGlideInfo( const char *szType, GlideInfo &rkInfo, ioINILoader &rkLoader );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsAttackEndState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pChar );
	void SetGlideState( ioBaseChar *pChar, bool bLandSkill, bool bJumpKey );
	void SetEndState( ioBaseChar *pChar );

	void ProcessLoopState( ioBaseChar *pChar );
	void ProcessGlideState( ioBaseChar *pChar );

	bool CheckLanding( ioBaseChar *pChar );

protected:
	inline void SetSkillState( SkillState eState ) { m_SkillState = eState; }
	const GlideInfo &GetGlideInfo() const;

public:
	ioCountForceSkill();
	ioCountForceSkill( const ioCountForceSkill &rhs );
	virtual ~ioCountForceSkill();
};

inline ioCountForceSkill* ToCountForceSkill( ioSkill *pSkill )
{
	ioCountSkill *pRange = ToCountSkill( pSkill );
	if( !pRange || pRange->GetSubSkillType() != CST_FORCE )
		return NULL;

	return dynamic_cast< ioCountForceSkill* >(pRange);
}