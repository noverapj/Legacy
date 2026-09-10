#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioWhirlwndAttack : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PRE,
		SS_LOOP,
		SS_LOOP_END_ATTACK,
		SS_FAST_MOVE_PRE,
		SS_FAST_MOVE,
		SS_FAST_MOVE_END,
		SS_WHIRLWND_ATTACK,
		SS_CHOP,
		SS_WALL,
		SS_END,
	};

	enum SkillSyncState
	{
		SSS_LOOP_END_ATTACK,
		SSS_CHOP,
	};

protected:
	SkillState			m_SkillState;
	DWORD				m_dwSkillProtectTime;

protected:
	ioHashString		m_szLoopAnimation;
	float				m_fLoopAniRate;
	
	DWORD				m_dwLoopStartTime;	
	DWORD				m_dwLoopDuration;

protected:	
	float				m_fCircleRange;
	float				m_fCircleMoveSpeed;
	float				m_fCircleHeightGap;
	float				m_fCircleOffSet;
	bool				m_bCircleOn;

	D3DXVECTOR3			m_vCircleStartPos;

	ioHashString		m_szOnCircle;
	ioHashString		m_szOffCircle;
	ioHashString		m_szCurCircle;

	ioHashString		m_szRedGuideCircle;
	ioHashString		m_szBlueGuideCircle;

protected:
	AttackAttribute		m_LoopEndAttack;
	ioHashString		m_szLoopEndAttackEffect;

	float				m_fLoopEndAttackEffectYOffset;
	float				m_fLoopEndAttackEffectMoveSpeed;

	UniqueObjID			m_LoopEndAttackEffectID;

	D3DXVECTOR3			m_vAttackPos;
	D3DXVECTOR3			m_vAttackDir;

protected:
	ioHashString		m_szPreFastMove;
	float				m_fPreFastMoveRate;
	DWORD				m_dwPreFastMoveEndTime;
	DWORD				m_dwWhirlFastMoveIndex;

protected:
	float				m_fFastMoveSpeed;
	float				m_fMaxFastMoveRange;
	float				m_fCurFastMoveRange;

	ioHashString		m_szFastMoveMapEffect;
	UniqueObjID			m_FastMoveMapEffectID;

protected:
	ioHashString		m_szFastMoveEnd;
	float				m_fFastMoveEndRate;
	DWORD				m_dwFastMoveEndEndTime;

protected:
	WeaponInfo			m_WhirlAttackInfo;

	ioHashString		m_szWhirlAttack;
	float				m_fWhirlAttackRate;
	float				m_fMoveSpeed;

	DWORD				m_dwWhirlAttackDuration;	
	DWORD				m_dwWhirlAttackEndTime;
	DWORD				m_dwWhirlAttackIndex;

	DWORD				m_dwCharRotateTime;	

protected:
	AttackAttribute		m_WallEndAttack;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	void CheckWeaponItemMesh( ioBaseChar* pChar );

	void CheckPreState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckCircle( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckLoopEndState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckMovePreState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckFastMoveState( ioBaseChar* pChar, ioPlayStage* pStage );	
	void CheckFastEndState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckWhirlAttackState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckChopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckWallState( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	bool CheckWeaponLive( ioBaseChar* pChar, ioPlayStage* pStage, DWORD dwWeaponIdx );

protected:
	void ProcessCharRotate( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void SetLoopState( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetLoopEndAttackState( ioBaseChar* pChar, ioPlayStage* pStage, bool bSync = true );

	void SetFastMovePre( ioBaseChar* pChar, ioPlayStage* pStage );	
	void SetFastMove( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetFastEnd( ioBaseChar* pChar, ioPlayStage* pStage );
	
	void SetWhirlAttack( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetChopAttack( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetWallAttack( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetEnd( ioBaseChar* pChar, ioPlayStage* pStage );

protected:
	const ioHashString& GetMagicGuide( ioBaseChar *pChar );

public:
	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkipState() const;
	virtual bool IsNoDropState() const;	

public:
	virtual AttackAttribute& GetAttribute();
	virtual const AttackAttribute& GetAttributeConst() const;

public:
	ioWhirlwndAttack();
	ioWhirlwndAttack( const ioWhirlwndAttack &rhs );
	virtual ~ioWhirlwndAttack();
};

inline ioWhirlwndAttack* ToWhirlwndAttack( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_WHIRLWND_ATTACK )
		return NULL;

	return dynamic_cast< ioWhirlwndAttack* >(pAttack);
}

