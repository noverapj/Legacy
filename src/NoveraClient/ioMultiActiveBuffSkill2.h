#pragma once

#include "ioBuffSkill.h"

class ioBaseChar;
class ioINILoader;
class ioBuff;

class ioMultiActiveBuffSkill2 : public ioBuffSkill
{
public:
	enum ActiveSkillState
	{
		ASS_NONE,
		ASS_Target_Drop,
		ASS_No_Mark,
		ASS_Mark_Attack,
		ASS_Check_Mark,
		ASS_Move,
		ASS_Grap_Attack,
		ASS_Grap_Loop,
		ASS_Swing,
		ASS_Rotate,
		ASS_End,
	};
	
protected:
	ActiveSkillState m_ActiveState;

	ioHashString m_szTargetName;
	ioHashString m_szGrapTargetName;
	
	DWORD m_dwGrapStartTime;
		
protected:		
	ioHashString m_szStateCheckBuff;
	ioHashString m_szTargetCheckBuff;

	AttackAttribute	m_MarkAttack;
	AttackAttribute	m_GrapAttack;
	AttackAttribute	m_SwingAttack;

	ioHashString m_szMoveAni;
	ioHashString m_szGrapAni;

	float m_fMoveSpeed;
	float m_fArriveDist;

	float m_fCharHeightRate;
	float m_fGrapTargetRange;

	DWORD m_dwGrapCheckTime;
	float m_fCheckDist;

	float m_fNeedGrapGauge;
	float m_fNeedSwingGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsEndState() const;

	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	virtual bool IsEnableMultiActive( ioBaseChar *pChar );

	virtual bool IsSkipState() const;

	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

protected:
	void SetMarkAttackState( ioBaseChar *pOwner );
	void SetMoveState( ioBaseChar *pOwner );
	void SetGrapAttackState( ioBaseChar *pOwner );
	void SetGrapLoopState( ioBaseChar *pOwner );
	void SetSwingState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pOwner );

protected:
	bool IsMarkBuff( ioBaseChar *pOwner );
	void CheckMove( ioBaseChar *pOwner, ioPlayStage *pStage );
	bool IsArrived( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckInputKey( ioBaseChar *pOwner, ioPlayStage *pStage );

public:
	ioMultiActiveBuffSkill2();
	ioMultiActiveBuffSkill2( const ioMultiActiveBuffSkill2 &rhs );
	virtual ~ioMultiActiveBuffSkill2();
};

inline ioMultiActiveBuffSkill2* ToMultiActiveBuffSkill2( ioSkill *pSkill )
{
	ioBuffSkill *pBuff = ToBuffSkill( pSkill );
	if( !pBuff || pBuff->GetSubSkillType() != BST_MULTI_ACTIVE2 )
		return NULL;

	return dynamic_cast< ioMultiActiveBuffSkill2* >( pSkill );
}